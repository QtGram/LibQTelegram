#include "mtprotoservicehandler.h"
#include "../crypto/gzip.h"
#include "../config/telegramconfig.h"
#include <QRegularExpression>
#include <QDateTime>

MTProtoServiceHandler::MTProtoServiceHandler(int dcid, QObject *parent) : QObject(parent), _dcid(dcid)
{
}

bool MTProtoServiceHandler::handle(MTProtoReply *mtreply)
{
    switch(mtreply->constructorId())
    {
        case TLTypes::MsgContainer:
            this->handleMsgContainer(mtreply);
            break;

        case TLTypes::GzipPacked:
            this->handleGzipPacked(mtreply);
            break;

        case TLTypes::RpcResult:
            this->handleRpcResult(mtreply);
            break;

        case TLTypes::RpcError:
            this->handleRpcError(mtreply);
            break;

        case TLTypes::NewSessionCreated:
            this->handleNewSessionCreated(mtreply);
            break;

        case TLTypes::MsgsAck:
            this->handleMsgAck(mtreply);
            break;

        case TLTypes::BadMsgNotification:
        case TLTypes::BadServerSalt:
            this->handleBadMsgNotification(mtreply);
            break;

        default:
            return false;
    }

    return true;
}

void MTProtoServiceHandler::checkRepeat(BadMsgNotification *badmsgnotification, TLLong servermsgid)
{
    switch(badmsgnotification->errorCode())
    {
        case 16:
        case 17:
        case 19:
        case 32:
        case 33:
        case 64:
        {
           TLInt servertime = ServerTime(servermsgid);
           emit deltaTimeChanged(CurrentDeltaTime(servertime), badmsgnotification->badMsgId());
           break;
        }

        default:
            break;
    }
}

void MTProtoServiceHandler::handleMsgContainer(MTProtoReply *mtreply)
{
    MessageContainer mc;
    mc.read(mtreply);

    foreach(MTProtoMessage* message, mc.messages())
    {
        MTProtoReply mtreplymsg(message, this->_dcid);
        emit serviceHandled(&mtreplymsg);
    }
}

void MTProtoServiceHandler::handleRpcResult(MTProtoReply *mtreply)
{
    RpcResult rr;
    rr.read(mtreply);

    MTProtoReply mtreplymsg(rr.result(), rr.reqMsgId(), this->_dcid);
    emit serviceHandled(&mtreplymsg);
}

void MTProtoServiceHandler::handleRpcError(MTProtoReply *mtreply)
{
    RpcError re;
    re.read(mtreply);

    emit ackRequest(mtreply->messageId());

    if(re.errorMessage().contains("_MIGRATE_"))
    {
        QRegularExpression regexp("_MIGRATE_([0-9]+)");
        QRegularExpressionMatch match = regexp.match(re.errorMessage());
        QString dcnum = match.captured(1);

        if(dcnum.isNull())
        {
            qFatal("DC %d Unknown destination DC: %s", this->_dcid, qUtf8Printable(dcnum));
            return;
        }

        emit migrateDC(this->_dcid, dcnum.toInt());
    }
    else if(re.errorMessage().contains("AUTH_KEY_"))
    {
        qDebug("DC %d Unauthorized client, requesting new authorization...", this->_dcid);
        emit unauthorized();
    }
    else if(re.errorMessage().contains("FLOOD_WAIT_"))
    {
        QRegularExpression regexp("FLOOD_WAIT_([0-9]+)");
        QRegularExpressionMatch match = regexp.match(re.errorMessage());
        QString seconds = match.captured(1);

        if(!seconds.isNull())
        {
            QDateTime unlockdate = QDateTime::currentDateTime().addSecs(seconds.toInt());
            qWarning("DC %d Flood lock enabled (unlock at: %s)", this->_dcid, qUtf8Printable(unlockdate.toString(Qt::SystemLocaleLongDate)));
            emit floodLock(seconds.toInt());
        }
        else
            qFatal("DC %d Cannot get flood duration", this->_dcid);
    }
    else if(re.errorMessage().startsWith("PHONE_CODE_"))
    {
        QString errormessage;

        if(re.errorMessage().endsWith("EMPTY"))
            errormessage = tr("Phone code empty");
        else if(re.errorMessage().endsWith("EXPIRED"))
            errormessage = tr("Phone code expired");
        else if(re.errorMessage().endsWith("INVALID"))
            errormessage = tr("Invalid phone code");
        else
            errormessage = tr("Unknown error");

        emit phoneCodeError(errormessage);
    }
    else if(re.errorMessage() == "PASSWORD_HASH_INVALID")
        emit invalidPassword();
    else if(re.errorMessage() == "SESSION_PASSWORD_NEEDED")
        emit sessionPasswordNeeded();
    else
        qWarning("DC %d (%llx) RPC Error %d %s", this->_dcid, mtreply->messageId(), re.errorCode(), qUtf8Printable(re.errorMessage()));
}

void MTProtoServiceHandler::handleBadMsgNotification(MTProtoReply *mtreply)
{
    BadMsgNotification badmsgnotification;
    badmsgnotification.read(mtreply);

    if(badmsgnotification.constructorId() == BadMsgNotification::CtorBadMsgNotification)
    {
        if(badmsgnotification.errorCode() == 16)
            qWarning("DC %d BadMsgNotification: msg_id too low", this->_dcid);
        else if(badmsgnotification.errorCode() == 17)
            qWarning("DC %d BadMsgNotification: msg_id too high", this->_dcid);
        else if(badmsgnotification.errorCode() == 18)
            qWarning("DC %d BadMsgNotification: Incorrect two lower order msg_id bits", this->_dcid);
        else if(badmsgnotification.errorCode() == 19)
            qWarning("DC %d BadMsgNotification: Container msg_id is the same as msg_id of a previously received message", this->_dcid);
        else if(badmsgnotification.errorCode() == 20)
            qWarning("DC %d BadMsgNotification: Message too old", this->_dcid);
        else if(badmsgnotification.errorCode() == 32)
            qWarning("DC %d BadMsgNotification: msg_seqno too low", this->_dcid);
        else if(badmsgnotification.errorCode() == 33)
            qWarning("DC %d BadMsgNotification: msg_seqno too high", this->_dcid);
        else if(badmsgnotification.errorCode() == 34)
            qWarning("DC %d BadMsgNotification: Even msg_seqno expected", this->_dcid);
        else if(badmsgnotification.errorCode() == 35)
            qWarning("DC %d BadMsgNotification: Odd msg_seqno expected", this->_dcid);
        else if(badmsgnotification.errorCode() == 48)
            qWarning("DC %d BadMsgNotification: Incorrect Server Salt", this->_dcid);
        else if(badmsgnotification.errorCode() == 64)
            qWarning("DC %d BadMsgNotification: Invalid container", this->_dcid);
        else
            qWarning("DC %d BadMsgNotification: unknown error code %d", this->_dcid, badmsgnotification.errorCode());

        emit ackRequest(badmsgnotification.badMsgId());
        this->checkRepeat(&badmsgnotification, mtreply->messageId());
    }
    else if(badmsgnotification.constructorId() == BadMsgNotification::CtorBadServerSalt)
    {
        qDebug("DC %d New salt %llx", this->_dcid, badmsgnotification.newServerSalt());

        emit ackRequest(badmsgnotification.badMsgId());
        emit saltChanged(badmsgnotification.newServerSalt(), badmsgnotification.badMsgId());
    }
}

void MTProtoServiceHandler::handleGzipPacked(MTProtoReply *mtreply)
{
    MTProtoObject mtobj;
    mtobj.read(mtreply);

    TLBytes packeddata = mtobj.packedData();
    TLBytes unpackeddata = GZip::uncompress(packeddata);
    MTProtoReply mtreplymsg(unpackeddata, mtreply->messageId(), this->_dcid);
    emit serviceHandled(&mtreplymsg);
}

void MTProtoServiceHandler::handleNewSessionCreated(MTProtoReply *mtreply)
{
    Q_UNUSED(mtreply);
    NewSession newsession;
    newsession.read(mtreply);

    qDebug("DC %d New Session (first_msg_id: %llx)", this->_dcid, newsession.firstMsgId());
}

void MTProtoServiceHandler::handleMsgAck(MTProtoReply *mtreply)
{
    Q_UNUSED(mtreply);

    MsgsAck msgsack;
    msgsack.read(mtreply);

    emit ack(msgsack.msgIds());
}

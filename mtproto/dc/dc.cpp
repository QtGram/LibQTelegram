#include "dc.h"
#include "../../config/telegramconfig.h"
#include "../mtprotoupdatehandler.h"
#include "../mtprotoreply.h"
#include <QDateTime>
#include <QTimerEvent>

TLLong DC::_lastclientmsgid = 0;

DC::DC(const QString &address, qint16 port, int dcid, QObject *parent): DCConnection(address, port, dcid, parent), _mtdecompiler(NULL), _savedrequest(NULL), _lastpacketlen(0), _contentmsgno(-1), _lastmsgid(0), _ownedsessions(0), _timcloseconnection(0)
{
    this->_mtservicehandler = new MTProtoServiceHandler(dcid, this);

    connect(this->_mtservicehandler, &MTProtoServiceHandler::ackRequest, this, &DC::onAckRequest);
    connect(this->_mtservicehandler, &MTProtoServiceHandler::migrateDC, this, &DC::migrateDC);
    connect(this->_mtservicehandler, &MTProtoServiceHandler::sessionPasswordNeeded, this, &DC::sessionPasswordNeeded);
    connect(this->_mtservicehandler, &MTProtoServiceHandler::invalidPassword, this, &DC::invalidPassword);
    connect(this->_mtservicehandler, &MTProtoServiceHandler::phoneCodeError, this, &DC::phoneCodeError);
    connect(this->_mtservicehandler, &MTProtoServiceHandler::saltChanged, this, &DC::onServerSaltChanged);
    connect(this->_mtservicehandler, &MTProtoServiceHandler::deltaTimeChanged, this, &DC::onDeltaTimeChanged);
    connect(this->_mtservicehandler, &MTProtoServiceHandler::ack, this, &DC::onAck);
    connect(this->_mtservicehandler, &MTProtoServiceHandler::floodLock, this, &DC::onDcFloodClock);
    connect(this->_mtservicehandler, &MTProtoServiceHandler::unauthorized, this, &DC::onDCUnauthorized);
    connect(this->_mtservicehandler, SIGNAL(serviceHandled(MTProtoReply*)), this, SLOT(handleReply(MTProtoReply*)));

    connect(this, &DC::connected, this, &DC::onDCConnected);
    connect(this, &DC::readyRead, this, &DC::onDCReadyRead);
}

MTProtoRequest *DC::lastRequest() const
{
    if(!this->_pendingrequests.contains(this->_lastmsgid))
        return NULL;

    return this->_pendingrequests[this->_lastmsgid];
}

void DC::sendPlain(MTProtoStream *mtstream)
{
    MTProtoRequest req(this->id());
    req.setBody(mtstream); // Take ownership

    this->send(&req);
}

TLInt DC::generateContentMsgNo()
{
    this->_contentmsgno = this->_contentmsgno + 2;
    return this->_contentmsgno;
}

void DC::assignMessageId(MTProtoRequest* req)
{
    DCConfig& dcconfig = DCConfig_fromDcId(this->id());
    TLLong unixtime = (static_cast<TLLong>(QDateTime::currentDateTime().toTime_t()) - dcconfig.deltaTime()) << 32LL;
    TLLong msgid = 0, ticks = 4 - (unixtime % 4);

    if(!(unixtime % 4))
        msgid = unixtime;
    else
        msgid = unixtime + ticks;

    DC::_lastclientmsgid = this->_lastmsgid = msgid = qMax(msgid, DC::_lastclientmsgid + 4);

    if((msgid % 4) != 0)
        qFatal("Message %llx not divisible by 4 (yields %lld)", msgid, msgid % 4);

    req->setMessageId(msgid);
}

void DC::checkSyncronization(MTProtoReply *mtreply)
{
    DCConfig& dcconfig = DCConfig_fromDcId(this->id());

    TLLong servertime = mtreply->messageId() >> 32LL;
    TLLong clienttime = QDateTime::currentDateTime().toTime_t() - dcconfig.deltaTime();

    if(clienttime <= (servertime - 30))
        qWarning("DC %d Message %llx has a date at least 30 seconds in the future than current date", this->id(), mtreply->messageId());
    else if(clienttime >= (servertime + 300))
        qWarning("DC %d Message %llx was sent at least 300 seconds ago", this->id(), mtreply->messageId());
}

void DC::decompile(int direction, TLLong messageid, const QByteArray& body)
{
    if(!TelegramConfig::config()->debugMode())
        return;

    if(!this->_mtdecompiler)
        this->_mtdecompiler = new MTProtoDecompiler();

    this->_mtdecompiler->decompile(this->id(), direction, messageid, body);
}

TLInt DC::getPacketLength()
{
    uchar len = 0;
    this->getChar(reinterpret_cast<char*>(&len));

    if(len < 0x7F)
        return len * 4;
    else if(len == 0x7F)
    {
        TLInt packetlength = 0;
        this->read(reinterpret_cast<char*>(&packetlength), 3);
        return packetlength * 4;
    }

    qFatal("DC %d Incorrect TCP Package", this->id());
    return 0;
}

void DC::repeatRequest(TLLong msgid)
{
    if(!this->_pendingrequests.contains(msgid))
    {
        qDebug("DC %d Expired request %llx...", this->id(), msgid);
        return;
    }

    qDebug("DC %d Repeating request %llx...", this->id(), msgid);

    MTProtoRequest* req = this->_pendingrequests.take(msgid);
    req->setAcked(false);
    this->send(req);
}

void DC::onDCConnected()
{
    this->_contentmsgno = -1; // Init connection to DC
    MTProtoRequest::resetFirst(this->id());
}

void DC::onDCUnauthorized()
{
    // Reset authorization state
    DCConfig& dcconfig = DCConfig_fromDcId(this->id());
    dcconfig.reset();

    qDeleteAll(this->_pendingrequests);
    this->_pendingrequests.clear();

    this->abort();
    emit unauthorized();
}

void DC::onDcFloodClock(int seconds)
{
    this->_pendingrequests.clear();

    this->abort();
    emit floodLock(seconds);
}

void DC::handleReply(const QByteArray &message)
{
    MTProtoReply mtreply(message, this->id());

    if(mtreply.isError())
    {
        qWarning("DC %d ERROR %d", this->id(), qAbs(mtreply.errorCode()));

        if(!this->_pendingrequests.contains(this->_lastmsgid))
            return;

        MTProtoRequest* req = this->_pendingrequests.take(this->_lastmsgid);
        req->setAcked(true);
        req->error();
        req->deleteLater();
        return;
    }

    if(((mtreply.messageId() % 4) != 1) && ((mtreply.messageId() % 4) != 3))
        qFatal("DC %d: Invalid server Message %llx (yields %lld, instead of 1 or 3)", this->id(), mtreply.messageId(), mtreply.messageId() % 4);

    this->handleReply(&mtreply);
}

void DC::handleReply(MTProtoReply *mtreply)
{
    if(this->_mtservicehandler->handle(mtreply))
        return;

    DCConfig& dcconfig = DCConfig_fromDcId(this->id());
    MTProtoRequest* req = this->_pendingrequests.take(mtreply->messageId());

    if(req)
        req->setAcked(true);

    if(dcconfig.authorization() >= DCConfig::Authorized)
    {
        this->checkSyncronization(mtreply);
        bool handled = UpdateHandler_instance->handle(mtreply);

        if(!handled)
            this->decompile(MTProtoDecompiler::DIRECTION_IN, mtreply->messageId(), mtreply->cbody());
        else
            mtreply->seekToBody(); // Prepare buffer to be handled in "replied" slots

        if(req)
            emit req->replied(mtreply);
        else if(!handled)
            qWarning("DC %d Request for msg_id %llx not found", this->id(), mtreply->messageId());
    }
    else
        emit authorizationReply(mtreply);

    if(req)
        req->deleteLater();
}

void DC::onAck(const TLVector<TLLong> &msgids)
{
    foreach(TLLong msgid, msgids)
    {
        if(!this->_pendingrequests.contains(msgid))
            continue;

        MTProtoRequest* req = this->_pendingrequests[msgid];
        req->setAcked(true);
    }
}

void DC::onAckRequest(TLLong reqmsgid)
{
    if(!this->_pendingrequests.contains(reqmsgid))
    {
        qWarning("DC %d Cannot ACK request %llx", this->id(), reqmsgid);
        return;
    }

    this->_pendingrequests[reqmsgid]->setAcked(true); // Don't repeat, we have received a reply
}

void DC::onServerSaltChanged(TLLong newserversalt, TLLong reqmsgid)
{
    DCConfig& config = DCConfig_fromDcId(this->id());
    config.setServerSalt(newserversalt);
    TelegramConfig_save;

    this->repeatRequest(reqmsgid);
}

void DC::onDeltaTimeChanged(TLLong deltatime, TLLong reqmsgid)
{
    DCConfig& dcconfig = DCConfig_fromDcId(this->id());
    dcconfig.setDeltaTime(deltatime);
    TelegramConfig_save;

    DC::_lastclientmsgid = 0;
    this->repeatRequest(reqmsgid);
}

void DC::send(MTProtoRequest *req)
{
    if(this->state() != DC::ConnectedState)
    {
        qWarning("DC %d Not connected, cannot send queries", this->id());
        return;
    }

    this->assignMessageId(req);

    if(req->encrypted())
    {
        this->_pendingrequests[req->messageId()] = req;
        DCConfig& dcconfig = DCConfig_fromDcId(this->id());

        if(dcconfig.authorization() < DCConfig::Authorized)
        {
            qWarning("DC %d Cannot send encrypted requests", this->id());
            return;
        }

        req->setSeqNo(this->generateContentMsgNo());
    }

    QByteArray reqpayload = req->build();
    this->decompile(MTProtoDecompiler::DIRECTION_OUT, req->messageId(), req->body());

    if(req->encrypted())
    {
        connect(req, &MTProtoRequest::timeout, this, &DC::repeatRequest, Qt::UniqueConnection);
        req->startTimer(QueryTimeout);
    }

    this->write(reqpayload);
}

void DC::keepRequest(MTProtoRequest *req)
{
    if(!req)
    {
        this->_savedrequest = NULL;
        return;
    }

    req->setAcked(false);
    req->setDcId(this->id());

    this->_savedrequest = req;
}

MTProtoRequest *DC::giveRequest()
{
    MTProtoRequest* req = this->_savedrequest;

    this->_savedrequest = NULL;
    return req;
}

void DC::addSessionRef()
{
    this->_ownedsessions++;
}

void DC::removeSessionRef()
{
    this->_ownedsessions--;

    Q_ASSERT(this->_ownedsessions >= 0);

    if(this->_ownedsessions == 0)
    {
        if(DCConfig_mainDcId == this->id() || (this->_timcloseconnection > 0)) // Main DC or timer is already running
            return;

        if(this->_pendingrequests.count() > 0)
            this->freeOwnedRequests();

        this->_timcloseconnection = this->startTimer(CloseDCTimeout);
    }
}

void DC::timerEvent(QTimerEvent *event)
{
    DCConnection::timerEvent(event);

    if(event->timerId() == this->_timcloseconnection)
    {
        if(this->_pendingrequests.count() <= 0)
            this->close();

        killTimer(event->timerId());
        this->_timcloseconnection = 0;
    }
}

void DC::freeOwnedRequests()
{
    QList<MTProtoRequest*> requests = this->_pendingrequests.values();

    foreach(MTProtoRequest* req, requests)
    {
        if(req->dcId() != this->id())
            continue;

        req->deleteLater();
    }
}

void DC::onDCReadyRead()
{
    while(this->bytesAvailable())
    {
        TLInt packetlen = this->_lastpacketlen;  // Store message length (if any)

        if(!packetlen)
            packetlen = this->getPacketLength(); // First message: read length

        if(packetlen < 4)
        {
            qFatal("DC %d Invalid TCP package, length: %d", this->id(), packetlen);
            return;
        }

        if(this->bytesAvailable() < packetlen)   // Incomplete message: store message length and wait for more data
        {
            this->_lastpacketlen = packetlen;
            return;
        }

        QByteArray message = this->read(packetlen);
        this->handleReply(message);
        this->_lastpacketlen = 0;
    }
}

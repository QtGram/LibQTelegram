#include "dc.h"
#include "../../config/telegramconfig.h"
#include "../mtprotoupdatehandler.h"
#include "../mtprotoreply.h"

DC::DC(const QString &address, qint16 port, int dcid, QObject *parent): DCConnection(address, port, parent), _mtdecompiler(NULL), _lastpacketlen(0), _contentmsgno(-1), _lastmsgid(0), _dcid(dcid)
{
    this->_mtservicehandler = new MTProtoServiceHandler(dcid, this);

    connect(this->_mtservicehandler, &MTProtoServiceHandler::configurationReceived, this, &DC::configurationReceived);
    connect(this->_mtservicehandler, &MTProtoServiceHandler::migrateDC, this, &DC::migrateDC);
    connect(this->_mtservicehandler, &MTProtoServiceHandler::saltChanged, this, &DC::repeatLastRequest);
    connect(this->_mtservicehandler, SIGNAL(serviceHandled(MTProtoReply*)), this, SLOT(handleReply(MTProtoReply*)));
    connect(this, &DC::connected, this, &DC::sendPendingRequests);
    connect(this, &DC::readyRead, this, &DC::onDCReadyRead);
}

TLInt DC::generateContentMsgNo()
{
    this->_contentmsgno = this->_contentmsgno + 2;
    return this->_contentmsgno;
}

int DC::id() const
{
    return this->_dcid;
}

void DC::decompile(int direction, TLLong messageid, const QByteArray& body)
{
    if(!TelegramConfig::config()->debugMode())
        return;

    if(!this->_mtdecompiler)
        this->_mtdecompiler = new MTProtoDecompiler();

    this->_mtdecompiler->decompile(this->_dcid, direction, messageid, body);
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

    qFatal("Incorrect TCP Package");
    return 0;
}

void DC::repeatLastRequest()
{
    if(!this->_sentrequests.contains(this->_lastmsgid))
        return;

    this->send(this->_sentrequests[this->_lastmsgid]);
}

void DC::handleReply(const QByteArray &message)
{
    MTProtoReply mtreply(message, this->_dcid);

    if(mtreply.isError())
    {
        qDebug() << "DC" << this->_dcid << "ERROR" << qAbs(mtreply.errorCode());
        return;
    }

    if(((mtreply.messageId() % 4) != 1) && ((mtreply.messageId() % 4) != 3))
        qFatal("Invalid server MessageId %llx (yields %lld, instead of 1 or 3)", mtreply.messageId(), mtreply.messageId() % 4);

    TLInt servertime = mtreply.messageId() >> 32;
    DCConfig& dcconfig = DcConfig_fromDc(this);

    dcconfig.setServerTime(servertime);
    this->handleReply(&mtreply);
}

void DC::handleReply(MTProtoReply *mtreply)
{
    if(this->_mtservicehandler->handle(mtreply))
        return;

    if(UpdateHandler_instance->handle(mtreply))
        return;

    this->decompile(MTProtoDecompiler::DIRECTION_IN, mtreply->messageId(), mtreply->cbody());
    DCConfig& dcconfig = DcConfig_fromDcId(this->_dcid);

    if(dcconfig.authorization() < DCConfig::Authorized)
    {
        emit authorizationReply(mtreply);
        return;
    }

    MTProtoRequest* req = this->_sentrequests[mtreply->messageId()];

    if(!req)
    {
        qWarning("Request for msg_id %llx not found", mtreply->messageId());
        return;
    }

    emit req->replied(mtreply);

    this->_sentrequests.remove(mtreply->messageId());
    req->deleteLater();
}

void DC::sendPendingRequests()
{
    int idx = 0;
    DCConfig& dcconfig = DcConfig_fromDcId(this->_dcid);

    while(idx < this->_pendingrequests.length())
    {
        MTProtoRequest* req = this->_pendingrequests[idx];

        // Check for authorization state
        if(req->encrypted() && (dcconfig.authorization() < DCConfig::Authorized))
        {
            idx++;
            continue;
        }

        this->_pendingrequests.removeAt(idx);
        this->send(req);
    }
}

TLLong DC::send(MTProtoRequest *req)
{
    if(this->state() == DC::ConnectedState)
    {
        if(req->encrypted())
        {
            DCConfig& dcconfig = DcConfig_fromDcId(this->_dcid);

            if(dcconfig.authorization() < DCConfig::Authorized)
                return req->messageId();

            req->setSeqNo(this->generateContentMsgNo());
        }

        if(this->_sentrequests.contains(req->messageId()))
            this->_sentrequests.remove(req->messageId());

        QByteArray reqpayload = req->build();

        if(req->encrypted())
        {
            this->_sentrequests[req->messageId()] = req;
            this->_lastmsgid = req->messageId();
        }

        this->decompile(MTProtoDecompiler::DIRECTION_OUT, req->messageId(), req->body());
        this->write(reqpayload);

        if(!req->encrypted())
            req->deleteLater(); // Plain requests are handled manually
    }
    else
    {
        this->_pendingrequests << req;

        if(this->state() == DC::UnconnectedState)
            this->connectToDC();
    }

    return req->messageId();
}

void DC::takeRequests(TLLong sessionid, TLLong* lastmsgid, DC *fromdc)
{
    QList<TLLong> messageids = fromdc->_sentrequests.keys();
    qSort(messageids); // Sort by messageid

    foreach(TLLong messageid, messageids)
    {
        MTProtoRequest* req = fromdc->_sentrequests[messageid];
        req->setDcId(this->_dcid);
        req->setLastMsgId(lastmsgid);
        req->setSessionId(sessionid);
        this->_pendingrequests << req;
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
            qFatal("Invalid TCP package, length: %d", packetlen);
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

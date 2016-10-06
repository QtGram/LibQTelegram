#include "dc.h"
#include "../../config/telegramconfig.h"
#include "../mtprotoreply.h"

DC::DC(const QString &address, qint16 port, int dcid, QObject *parent): DCConnection(address, port, parent), _mtdecompiler(NULL), _lastpacketlen(0), _contentmsgno(0), _dcid(dcid)
{
    this->_mtservicehandler = new MTProtoServiceHandler(dcid, this);

    connect(this->_mtservicehandler, SIGNAL(serviceHandled(MTProtoReply*)), this, SLOT(handleReply(MTProtoReply*)));
    connect(this->_mtservicehandler, SIGNAL(configurationReceived(Config*)), this, SIGNAL(configurationReceived(Config*)));
    connect(this->_mtservicehandler, SIGNAL(migrateDC(int)), this, SIGNAL(migrateDC(int)));
    connect(this, &DC::connected, this, &DC::sendPendingRequests);
    connect(this, &DC::readyRead, this, &DC::onDCReadyRead);
}

TLInt DC::generateContentMsgNo()
{
    this->_contentmsgno = this->_contentmsgno * 2 + 1;
    return this->_contentmsgno;
}

int DC::id() const
{
    return this->_dcid;
}

MTProtoRequest *DC::popRequest(TLLong msgid)
{
    if(!this->_requests.contains(msgid))
        return NULL;

    return this->_requests.take(msgid);
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

void DC::handleReply(const QByteArray &message)
{
    MTProtoReply mtreply(message, this->_dcid);

    Q_ASSERT((mtreply.messageId() % 2) != 0);

    TLInt servertime = mtreply.messageId() >> 32;
    DCConfig& dcconfig = GET_DC_CONFIG_FROM_DC(this);

    dcconfig.setServerTime(servertime);
    this->handleReply(&mtreply);
}

void DC::handleReply(MTProtoReply *mtreply)
{
    if(mtreply->isError())
    {
        qDebug() << "ERROR:" << mtreply->errorCode();
        return;
    }

    if(this->_mtservicehandler->handle(mtreply))
        return;

    this->decompile(MTProtoDecompiler::DIRECTION_IN, mtreply->messageId(), mtreply->cbody());
    DCConfig& dcconfig = GET_DC_CONFIG_FROM_DCID(this->_dcid);

    if(dcconfig.authorization() != DCConfig::Authorized)
    {
        emit authorizationReply(mtreply);
        return;
    }

    //TODO: Free memory
    MTProtoRequest* req = this->_requests[mtreply->messageId()];

    if(!req)
    {
        qWarning("Request for msg_id %llx not found", mtreply->messageId());
        return;
    }

    MTProtoStream mtstream(mtreply->body());
    emit req->replied(&mtstream);
}

void DC::sendPendingRequests()
{
    int idx = 0;
    DCConfig& dcconfig = GET_DC_CONFIG_FROM_DCID(this->_dcid);

    for(int i = 0; i < this->_pendingrequests.length(); i++)
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
            this->_requests[req->mainMessageId()] = req;

            DCConfig& dcconfig = GET_DC_CONFIG_FROM_DCID(this->_dcid);

            if(dcconfig.authorization() < DCConfig::Authorized)
                return req->messageId();

            req->setSeqNo(this->generateContentMsgNo());
        }

        QByteArray reqpayload = req->build();
        this->decompile(MTProtoDecompiler::DIRECTION_OUT, req->messageId(), req->body());
        this->write(reqpayload);
    }
    else
    {
        this->_pendingrequests << req;

        if(this->state() == DC::UnconnectedState)
            this->connectToDC();
    }

    return req->messageId();
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

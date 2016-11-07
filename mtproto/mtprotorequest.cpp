#include "mtprotorequest.h"
#include "../config/telegramconfig.h"
#include "../autogenerated/tltypes.h"
#include "../crypto/hash.h"
#include "../crypto/aes.h"

QHash<int, bool> MTProtoRequest::_firstmap;

MTProtoRequest::MTProtoRequest(int dcid, QObject *parent) : QObject(parent), _acked(false), _dcid(dcid), _sessionid(0), _messageid(0), _seqno(0), _body(NULL)
{
    Try_InitFirst(dcid);
}

MTProtoRequest::~MTProtoRequest()
{

}

bool MTProtoRequest::acked() const
{
    return this->_acked;
}

int MTProtoRequest::dcId() const
{
    return this->_dcid;
}

TLLong MTProtoRequest::messageId() const
{
    return this->_messageid;
}

TLLong MTProtoRequest::sessionId() const
{
    return this->_sessionid;
}

bool MTProtoRequest::encrypted() const
{
    return this->_sessionid != 0;
}

const QByteArray &MTProtoRequest::body() const
{
    return this->_body->data();
}

void MTProtoRequest::setSessionId(TLLong sessionid)
{
    this->_sessionid = sessionid;
}

void MTProtoRequest::setMessageId(TLLong messageid)
{
    this->_messageid = messageid;
}

void MTProtoRequest::setSeqNo(TLInt seqno)
{
    this->_seqno = seqno;
}

void MTProtoRequest::setBody(MTProtoStream *body)
{
    body->setParent(this); // Take ownership

    this->_body = body;
}

QByteArray MTProtoRequest::build()
{
    QByteArray request, requestbody;

    if(this->encrypted())
        requestbody = this->buildEncrypted();
    else
        requestbody = this->buildPlain();

    this->build(request, requestbody);
    return request;
}

void MTProtoRequest::resetFirst(int dcid)
{
    ResetFirst(dcid);
}

void MTProtoRequest::setAcked(bool b)
{
    this->_acked = b;
}

void MTProtoRequest::setDcId(int dcid)
{
    this->_dcid = dcid;
}

void MTProtoRequest::initConnection(MTProtoStream &mtstream) const
{
    TelegramConfig* config = TelegramConfig_instance;

    mtstream.writeTLConstructor(TLTypes::InvokeWithLayer);
    mtstream.writeTLInt(config->layerNum());

    mtstream.writeTLConstructor(TLTypes::initConnection);
    mtstream.writeTLInt(config->apiId());
    mtstream.writeTLString(config->deviceModel());
    mtstream.writeTLString(config->osVersion());
    mtstream.writeTLString(config->applicationVersion());
    mtstream.writeTLString(QLocale::system().name());
}

void MTProtoRequest::build(QByteArray &request, const QByteArray &requestbody)
{
    if(IsFirst(this->_dcid))
    {
        request.append(0xEF); // Use Abridged version
        UnsetFirst(this->_dcid);
    }

    TLInt len = requestbody.length() / 4;

    if(len > 0x7E)
    {
        len |= 0x7F000000;
        request.append(reinterpret_cast<const char*>(&len), sizeof(TLInt));
    }
    else
        request.append(static_cast<qint8>(len));

    request.append(requestbody);
}

QByteArray MTProtoRequest::buildPlain()
{
    MTProtoStream mtstream;

    mtstream.writeTLLong(0);
    mtstream.writeTLLong(this->_messageid);
    mtstream.writeRaw(this->_body);

    return mtstream.data();
}

void MTProtoRequest::timerEvent(QTimerEvent *event)
{
    if(!this->_acked)
        emit timeout(this->_messageid);

    killTimer(event->timerId());
}

QByteArray MTProtoRequest::buildEncrypted()
{
    Q_ASSERT(this->_dcid > 0);
    Q_ASSERT(this->_sessionid != 0);
    Q_ASSERT(this->_seqno > 0);

    DCConfig& dcconfig = DCConfig_fromDcId(this->_dcid);
    MTProtoStream mtproto;

    mtproto.writeTLLong(dcconfig.serverSalt());
    mtproto.writeTLLong(this->_sessionid);
    mtproto.writeTLLong(this->_messageid);
    mtproto.writeTLInt(this->_seqno);

    if(this->_seqno == 1)
    {
        MTProtoStream header;
        this->initConnection(header);

        mtproto.writeTLInt(header.length() + this->_body->length());
        mtproto.writeRaw(&header, false);
        mtproto.writeRaw(this->_body, false);
    }
    else
        mtproto.writeRaw(this->_body);

    QByteArray aeskey, aesiv, msgkey = sha1_hash(mtproto.data()).right(16);
    Aes::calculateAesKeys(dcconfig.authorizationKey(), msgkey, false, aeskey, aesiv);
    mtproto.randPad(16);
    QByteArray encryptedmtproto = Aes::encrypt(mtproto.data(), aeskey, aesiv);

    MTProtoStream encryptedmessage;
    encryptedmessage.writeTLLong(dcconfig.authorizationKeyId());
    encryptedmessage.writeRaw(msgkey, false);
    encryptedmessage.writeRaw(encryptedmtproto, false);

    return encryptedmessage.data();
}

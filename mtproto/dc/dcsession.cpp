#include "dcsession.h"
#include "../../crypto/math.h"
#include <QDateTime>

DCSession::DCSession(DC *dc, QObject *parent) : QObject(parent), _lastmsgid(0), _dc(dc), _owneddc(false)
{
    this->_acktimer = new QTimer(this);
    this->_acktimer->setSingleShot(true);
    this->_acktimer->setInterval(1600);

    connect(this->_acktimer, &QTimer::timeout, this, &DCSession::sendAck);

    this->generateSessionId();
}

bool DCSession::ownedDc() const
{
    return this->_owneddc;
}

TLLong DCSession::sessionId() const
{
    return this->_sessionid;
}

TLLong *DCSession::lastMsgId()
{
    return &this->_lastmsgid;
}

DC *DCSession::dc() const
{
    return this->_dc;
}

void DCSession::setOwnedDC(bool b)
{
    this->_dc->setParent(b ? this : NULL);
}

MTProtoRequest *DCSession::sendPlain(MTProtoStream *mtstream)
{
    MTProtoRequest* req = new MTProtoRequest(&this->_lastmsgid, this->_dc->id());
    req->setBody(mtstream); // Take ownership

    this->_dc->send(req);
    return req;
}

MTProtoRequest *DCSession::sendEncrypted(MTProtoStream *mtstream)
{
    MTProtoRequest* req = new MTProtoRequest(&this->_lastmsgid, this->_dc->id());
    req->setSessionId(this->_sessionid);
    req->setBody(mtstream); // Take ownership

    connect(req, &MTProtoRequest::replied, this, &DCSession::queueAck);

    this->_dc->send(req);
    return req;
}

void DCSession::generateSessionId()
{
    Math::randomize(&this->_sessionid);
}

void DCSession::sendAck()
{
    MTProtoStream* mtstream = new MTProtoStream();
    mtstream->writeTLConstructor(TLTypes::MsgsAck);
    mtstream->writeTLVector(this->_ackqueue);

    MTProtoRequest* req = new MTProtoRequest(&this->_lastmsgid, this->_dc->id());
    req->setSessionId(this->_sessionid);
    req->setBody(mtstream); // Take ownership

    this->_dc->send(req);
    this->_ackqueue.clear();
    req->deleteLater();
}

void DCSession::queueAck(MTProtoReply *)
{
    MTProtoRequest* req = qobject_cast<MTProtoRequest*>(this->sender());

    if(!this->_acktimer->isActive())
        this->_acktimer->start();

    this->_ackqueue << req->messageId();

    if(this->_ackqueue.length() > 16)
    {
        this->_acktimer->stop();
        this->sendAck();
    }
}

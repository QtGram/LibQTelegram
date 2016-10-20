#include "dcsession.h"
#include "../../crypto/math.h"
#include <QDateTime>

DCSession::DCSession(DC *dc, QObject *parent) : QObject(parent), _dc(dc)
{
    this->_acktimer = new QTimer(this);
    this->_acktimer->setSingleShot(true);
    this->_acktimer->setInterval(AckTimeout);

    connect(dc, &DC::unauthorized, this, &DCSession::requestAuthorization);
    connect(dc, &DC::connected, this, &DCSession::requestAuthorization);
    connect(this->_acktimer, &QTimer::timeout, this, &DCSession::sendAck);
}

bool DCSession::ownedDc() const
{
    return this->_dc->parent() == this;
}

TLLong DCSession::sessionId() const
{
    return this->_sessionid;
}

DC *DCSession::dc() const
{
    return this->_dc;
}

void DCSession::setOwnedDC(bool b)
{
    this->_dc->setParent(b ? this : NULL);
}

void DCSession::sendPlain(MTProtoStream *mtstream)
{
    MTProtoRequest req(this->_dc->id());
    req.setBody(mtstream); // Take ownership

    this->_dc->send(&req);
}

MTProtoRequest *DCSession::sendEncrypted(MTProtoStream *mtstream)
{
    MTProtoRequest* req = new MTProtoRequest(this->_dc->id());
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

void DCSession::requestAuthorization()
{
    this->generateSessionId();
    this->_ackqueue.clear();

    emit unauthorized(this);
}

void DCSession::sendAck()
{
    MTProtoStream* mtstream = new MTProtoStream();
    mtstream->writeTLConstructor(TLTypes::MsgsAck);
    mtstream->writeTLVector(this->_ackqueue);

    MTProtoRequest* req = new MTProtoRequest(this->_dc->id());
    req->setSessionId(this->_sessionid);
    req->setBody(mtstream); // Take ownership

    this->_dc->send(req);
    this->_ackqueue.clear();
    req->deleteLater();
}

void DCSession::queueAck(MTProtoReply* mtreply)
{
    if(!this->_acktimer->isActive())
        this->_acktimer->start();

    this->_ackqueue << mtreply->messageId();

    if(this->_ackqueue.length() > 16)
    {
        this->_acktimer->stop();
        this->sendAck();
    }
}

#include "dcsession.h"
#include "../../crypto/math.h"
#include <QDateTime>

DCSession::DCSession(DC *dc, QObject *parent) : QObject(parent), _lastmsgid(0), _dc(dc), _owneddc(false)
{
    this->generateSessionId();
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
    MTProtoRequest* req = new MTProtoRequest(this->_lastmsgid, this->_dc->id(), this);
    req->setBody(mtstream);

    this->_dc->send(req);
    return req;
}

MTProtoRequest *DCSession::sendEncrypted(MTProtoStream *mtstream)
{
    MTProtoRequest* req = new MTProtoRequest(this->_lastmsgid, this->_dc->id(), this);
    req->setSessionId(this->_sessionid);
    req->setBody(mtstream);

    this->_dc->send(req);
    return req;
}

void DCSession::generateSessionId()
{
    Math::randomize(&this->_sessionid);
}

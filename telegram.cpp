#include "telegram.h"
#include "crypto/math.h"

Telegram::Telegram(QObject *parent) : QObject(parent), _apiid(0), _port(0), _dcid(0), _debugmode(false)
{
}

const QString &Telegram::publicKey() const
{
    return this->_publickey;
}

const QString &Telegram::host() const
{
    return this->_host;
}

qint32 Telegram::apiId() const
{
    return this->_apiid;
}

const QString &Telegram::apiHash() const
{
    return this->_apihash;
}

const QString &Telegram::phoneNumber() const
{
    return this->_phonenumber;
}

qint32 Telegram::port() const
{
    return this->_port;
}

qint32 Telegram::dcId() const
{
    return this->_dcid;
}

bool Telegram::debugMode() const
{
    return this->_debugmode;
}

void Telegram::setPublicKey(const QString &publickey)
{
    if(this->_publickey == publickey)
        return;

    this->_publickey = publickey;

    this->tryConnect();
    emit publicKeyChanged();
}

void Telegram::setHost(const QString &host)
{
    if(this->_host == host)
        return;

    this->_host = host;

    this->tryConnect();
    emit hostChanged();
}

void Telegram::setApiHash(const QString &apphash)
{
    if(this->_apihash == apphash)
        return;

    this->_apihash = apphash;
    this->tryConnect();
    emit apiHashChanged();
}

void Telegram::setPhoneNumber(const QString &phonenumber)
{
    if(this->_phonenumber == phonenumber)
        return;

    this->_phonenumber = phonenumber;
    this->tryConnect();
    emit phoneNumberChanged();
}

void Telegram::setApiId(qint32 appid)
{
    if(this->_apiid == appid)
        return;

    this->_apiid = appid;
    this->tryConnect();
    emit apiIdChanged();
}

void Telegram::setPort(qint32 port)
{
    if(this->_port == port)
        return;

    this->_port = port;

    this->tryConnect();
    emit portChanged();
}

void Telegram::setDcId(qint32 dcid)
{
    if(this->_dcid == dcid)
        return;

    this->_dcid = dcid;

    this->tryConnect();
    emit dcIdChanged();
}

void Telegram::setDebugMode(bool dbgmode)
{
    if(this->_debugmode == dbgmode)
        return;

    this->_debugmode = dbgmode;
    emit debugModeChanged();
}

void Telegram::tryConnect()
{
    if(this->_publickey.isEmpty() || this->_host.isEmpty() || this->_phonenumber.isEmpty() || this->_apihash.isEmpty())
        return;

    if(!this->_apiid || !this->_port || !this->_dcid)
        return;

    TelegramConfig::init(TELEGRAM_API_LAYER, this->_apiid, this->_apihash, this->_publickey, this->_phonenumber);
    TelegramConfig::config()->setDebugMode(true);

    DCSession* dcsession = DCSessionManager::instance()->createMainSession(this->_host, this->_port, this->_dcid);
//    MTProtoRequest* req = TelegramAPI::authSendCode(dcsession, this->_phonenumber, true, this->_apiid, this->_apphash);
    MTProtoRequest* req = TelegramAPI::authCheckPhone(dcsession, this->_phonenumber);
    connect(req, &MTProtoRequest::replied, this, &Telegram::onAuthCheckPhoneReplied);
}

void Telegram::onAuthCheckPhoneReplied(MTProtoStream *mtstream)
{
    qDebug() << "REPLY";
}

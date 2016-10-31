#include "telegraminitializer.h"
#include "mtproto/mtprotoupdatehandler.h"
#include "config/cache/cacheinitializer.h"
#include "config/cache/telegramcache.h"
#include <QDebug>

TelegramInitializer::TelegramInitializer(QObject *parent) : QObject(parent), _apiid(0), _port(0), _dcid(0), _debugmode(false), _floodwaittimer(0)
{
}

TelegramInitializer::~TelegramInitializer()
{
    if(!DCConfig_isLoggedIn)
        return;

    TelegramConfig_save;
}

const QString &TelegramInitializer::publicKey() const
{
    return this->_publickey;
}

const QString &TelegramInitializer::host() const
{
    return this->_host;
}

qint32 TelegramInitializer::apiId() const
{
    return this->_apiid;
}

const QString &TelegramInitializer::apiHash() const
{
    return this->_apihash;
}

const QString &TelegramInitializer::phoneNumber() const
{
    return this->_phonenumber;
}

qint32 TelegramInitializer::port() const
{
    return this->_port;
}

qint32 TelegramInitializer::dcId() const
{
    return this->_dcid;
}

bool TelegramInitializer::debugMode() const
{
    return this->_debugmode;
}

void TelegramInitializer::setPublicKey(const QString &publickey)
{
    if(this->_publickey == publickey)
        return;

    this->_publickey = publickey;

    this->tryConnect();
    emit publicKeyChanged();
}

void TelegramInitializer::setHost(const QString &host)
{
    if(this->_host == host)
        return;

    this->_host = host;

    this->tryConnect();
    emit hostChanged();
}

void TelegramInitializer::setApiHash(const QString &apphash)
{
    if(this->_apihash == apphash)
        return;

    this->_apihash = apphash;
    this->tryConnect();
    emit apiHashChanged();
}

void TelegramInitializer::setPhoneNumber(const QString &phonenumber)
{
    if(this->_phonenumber == phonenumber)
        return;

    this->_phonenumber = phonenumber;
    this->tryConnect();
    emit phoneNumberChanged();
}

void TelegramInitializer::setApiId(qint32 appid)
{
    if(this->_apiid == appid)
        return;

    this->_apiid = appid;
    this->tryConnect();
    emit apiIdChanged();
}

void TelegramInitializer::setPort(qint32 port)
{
    if(this->_port == port)
        return;

    this->_port = port;

    this->tryConnect();
    emit portChanged();
}

void TelegramInitializer::setDcId(qint32 dcid)
{
    if(this->_dcid == dcid)
        return;

    this->_dcid = dcid;
    emit dcIdChanged();
}

void TelegramInitializer::setDebugMode(bool dbgmode)
{
    if(this->_debugmode == dbgmode)
        return;

    this->_debugmode = dbgmode;
    emit debugModeChanged();
}

void TelegramInitializer::signIn(const QString &phonecode)
{
    MTProtoRequest* req = TelegramAPI::authSignIn(DC_MainSession, this->_phonenumber, this->_phonecodehash, phonecode);
    connect(req, &MTProtoRequest::replied, this, &TelegramInitializer::onLoginCompleted);
}

void TelegramInitializer::signUp(const QString &firstname, const QString &lastname, const QString &phonecode)
{
    MTProtoRequest* req = TelegramAPI::authSignUp(DC_MainSession, this->_phonenumber, this->_phonecodehash, phonecode, firstname, lastname);
    connect(req, &MTProtoRequest::replied, this, &TelegramInitializer::onLoginCompleted);
}

void TelegramInitializer::resendCode()
{
    if(this->_phonecodehash.isEmpty() || this->_phonecodehash.isEmpty())
        return;

    TelegramAPI::authResendCode(DC_MainSession, this->_phonenumber, this->_phonecodehash);
}

void TelegramInitializer::tryConnect()
{
    if(this->_publickey.isEmpty() || this->_host.isEmpty() || this->_phonenumber.isEmpty() || this->_apihash.isEmpty())
        return;

    if(!this->_apiid || !this->_port)
        return;

    TelegramConfig::init(TELEGRAM_API_LAYER, this->_apiid, this->_apihash, this->_publickey, this->_phonenumber);
    TelegramConfig::config()->setDebugMode(true);

    DCSession* mainsession = NULL;

    if(DCConfig_isLoggedIn)
    {
        TelegramCache_load;
        DCConfig& dcconfig = DCConfig_fromDcId(DCConfig_mainDcId);
        mainsession = DCSessionManager_instance->createMainSession(dcconfig);
        emit loginCompleted();
    }
    else
        mainsession = DCSessionManager_instance->createMainSession(this->_host, this->_port, this->_dcid);

    connect(DCSessionManager_instance, &DCSessionManager::floodWait, this, &TelegramInitializer::onFloodWait, Qt::UniqueConnection);
    connect(DCSessionManager_instance, &DCSessionManager::phoneCodeError, this, &TelegramInitializer::phoneCodeError, Qt::UniqueConnection);
    connect(DCSessionManager_instance, &DCSessionManager::sessionReady, this, &TelegramInitializer::onMainSessionReady, Qt::UniqueConnection);
    DC_InitializeSession(mainsession);
}

void TelegramInitializer::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == this->_floodwaittimer)
    {
        this->killTimer(this->_floodwaittimer);
        this->_floodwaittimer = 0;
    }
}

void TelegramInitializer::onMainSessionReady(DCSession* dcsession)
{
    if(dcsession != DC_MainSession)
        return;

    if(DCConfig_isLoggedIn)
    {
        UpdateHandler_sync;
        return;
    }

    MTProtoRequest* req = TelegramAPI::authSendCode(dcsession, this->_phonenumber, false, this->_apiid, this->_apihash);
    connect(req, &MTProtoRequest::replied, this, &TelegramInitializer::onAuthCheckPhoneReplied);
}

void TelegramInitializer::onAuthCheckPhoneReplied(MTProtoReply *mtreply)
{
    AuthSentCode sentcode;
    sentcode.read(mtreply);

    this->_phonecodehash = sentcode.phoneCodeHash();

    if(sentcode.isPhoneRegistered())
    {
        emit signInRequested();
        return;
    }

    emit signUpRequested();
}

void TelegramInitializer::onLoginCompleted(MTProtoReply *mtreply)
{
    DCConfig& dcconfig = DCConfig_fromDcId(mtreply->dcid());

    AuthAuthorization authorization;
    authorization.read(mtreply);

    dcconfig.setAuthorization(DCConfig::Signed);

    User* me = authorization.user();
    me->setParent(this);

    TelegramCache_store(me); // Cache "me"
    TelegramConfig_instance->setMe(me);
    TelegramConfig_save;

    CacheInitializer* cacheinitializer = new CacheInitializer(this);

    connect(cacheinitializer, &CacheInitializer::initialized, [this, cacheinitializer] {
        emit this->loginCompleted();
        cacheinitializer->deleteLater();
    });

    cacheinitializer->initialize();
    this->_phonecodehash.clear();
}

void TelegramInitializer::onFloodWait(int seconds)
{
    if(this->_floodwaittimer) // Ignore other flood signals for some time
        return;

    this->_floodwaittimer = this->startTimer(5000); // 5 seconds
    emit floodWait(seconds);
}

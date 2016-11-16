#include "telegramconfig.h"
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QHostInfo>

#define CONFIG_FOLDER "telegram"

const QString TelegramConfig::DCCONFIG_FILE = "dcconfig.json";
const QString TelegramConfig::CONFIG_FILE = "config.mtproto";
const QString TelegramConfig::STATE_FILE = "state.mtproto";
const QString TelegramConfig::ME_FILE = "me.user";

TelegramConfig* TelegramConfig::_instance = NULL;

TelegramConfig::TelegramConfig(QObject* parent): QObject(parent), _config(NULL), _me(NULL), _layernum(0), _debugmode(false), _isipv6(false)
{
    this->_updatesstate = new UpdatesState();
    this->_storagepath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QDir::separator() + CONFIG_FOLDER;
    this->_devicemodel = QHostInfo::localHostName();

    if(this->_devicemodel.isEmpty())
        this->_devicemodel = "Unknown Device";
}

TelegramConfig *TelegramConfig::config()
{
    if(!TelegramConfig::_instance)
        return NULL;

    return TelegramConfig::_instance;
}

TelegramConfig *TelegramConfig::init(TLInt layernum, TLInt apiid, const QString &apihash, const QString &publickey, const QString& phonenumber)
{
    if(TelegramConfig::_instance)
        return TelegramConfig::_instance;

    TelegramConfig::_instance = new TelegramConfig();
    TelegramConfig::_instance->_layernum = layernum;
    TelegramConfig::_instance->_apiid = apiid;
    TelegramConfig::_instance->_apihash = apihash;
    TelegramConfig::_instance->_publickey = publickey;
    TelegramConfig::_instance->setPhoneNumber(phonenumber);
    TelegramConfig::_instance->load();

    return TelegramConfig::_instance;
}

User *TelegramConfig::me()
{
    return this->_me;
}

DCConfig *TelegramConfig::createConfig(const QString &host, TLInt port, int dcid)
{
    DCConfig* dcconfig = new DCConfig(host, port, dcid, this->_isipv6, this);
    this->_dcconfig[dcconfig->id()] = dcconfig;
    return dcconfig;
}

DCConfig *TelegramConfig::migrateConfig(DCConfig *fromconfig, int todcid)
{
    DCConfig::Id configid = MakeDCConfigId(fromconfig->option()->isIpv6(),
                                           fromconfig->option()->isMediaOnly(),
                                           todcid);

    return this->_dcconfig[configid];
}

UpdatesState *TelegramConfig::updateState()
{
    return this->_updatesstate;
}

Config *TelegramConfig::serverConfig()
{
    return this->_config;
}

void TelegramConfig::setServerConfig(Config *config)
{
    config->setParent(this);

    this->_config = config;

    foreach(DcOption* dcoption, config->dcOptions())
    {
        DCConfig::Id id = MakeDCConfigId(dcoption->isIpv6(), dcoption->isMediaOnly(), dcoption->id());

        if(!this->_dcconfig.contains(id))
        {
            DCConfig* dcconfig = new DCConfig(dcoption, this);
            this->_dcconfig[id] = dcconfig;
        }
        else
            this->_dcconfig[id]->setDcOption(dcoption);
    }
}

DCConfig* TelegramConfig::mainConfig() const
{
    foreach(DCConfig* dcconfig, this->_dcconfig.values())
    {
        if(dcconfig->isMain())
            return dcconfig;
    }

    return NULL;
}

DCConfig *TelegramConfig::mediaDcConfig() const
{
    foreach(DCConfig* dcconfig, this->_dcconfig.values())
    {
        if(dcconfig->option()->isMediaOnly())
            return dcconfig;
    }

    qWarning("Cannot find media DC");
    return NULL;
}

DCConfig *TelegramConfig::dcConfig(DCConfig::Id id)
{
    return this->_dcconfig[id];
}

DCConfig *TelegramConfig::fromDcId(int dcid)
{
    return this->dcConfig(MakeDCConfigId(this->_isipv6, false, dcid));
}

void TelegramConfig::setMainConfig(DCConfig* dcconfig)
{
    foreach(DCConfig* currentconfig, this->_dcconfig.values())
        currentconfig->setIsMain(dcconfig == currentconfig);
}

void TelegramConfig::save()
{
    this->saveDCConfig();
    this->saveConfig();
    this->saveState();
    this->saveMe();
}

void TelegramConfig::load()
{
    this->loadMe();
    this->loadState();
    this->loadConfig();
    this->loadDCConfig();
}

void TelegramConfig::reset()
{
    this->_dcconfig.clear();
}

bool TelegramConfig::needsConfiguration() const
{
    return this->_dcconfig.count() <= 1;
}

bool TelegramConfig::isLoggedIn() const
{
    foreach(const DCConfig* dcconfig, this->_dcconfig.values())
    {
        if(dcconfig->authorization() == DCConfig::Signed)
            return true;
    }

    return false;
}

bool TelegramConfig::debugMode() const
{
    return this->_debugmode;
}

bool TelegramConfig::isIPv6() const
{
    return this->_isipv6;
}

TLInt TelegramConfig::layerNum() const
{
    return this->_layernum;
}

TLInt TelegramConfig::apiId() const
{
    return this->_apiid;
}

const QString &TelegramConfig::apiHash() const
{
    return this->_apihash;
}

const QString &TelegramConfig::publicKey() const
{
    return this->_publickey;
}

const QString &TelegramConfig::storagePath() const
{
    return this->_storagepath;
}

const QString &TelegramConfig::deviceModel() const
{
    return this->_devicemodel;
}

QString TelegramConfig::osVersion() const
{
    #if defined(Q_OS_ANDROID)
        return QString("Android");
    #elif defined(Q_OS_BLACKBERRY)
        return QString("Blackberry");
    #elif defined(Q_OS_IOS)
        return QString("iOS");
    #elif defined(Q_OS_MAC)
        return QString("MacOS");
    #elif defined(Q_OS_WINCE)
        return QString("WinCE");
    #elif defined(Q_OS_WIN)
        return QString("Windows");
    #elif defined(Q_OS_LINUX)
        return QString("Linux");
    #elif defined(Q_OS_UNIX)
        return QString("Unix");
    #else
        return QString("Unknown");
    #endif
}

QString TelegramConfig::applicationVersion() const
{
    return qApp->applicationVersion().isEmpty() ? "1.0" : qApp->applicationVersion();
}

const QString &TelegramConfig::phoneNumber() const
{
    return this->_phonenumber;
}

void TelegramConfig::setDebugMode(bool dbgmode)
{
    this->_debugmode = dbgmode;
}

void TelegramConfig::setIpv6(bool ipv6)
{
    this->_isipv6 = ipv6;
}

void TelegramConfig::setStoragePath(const QString &storagepath)
{
    this->updateStoragePath(storagepath, this->_phonenumber);
}

void TelegramConfig::setDeviceModel(const QString &devicemodel)
{
    this->_devicemodel = devicemodel;
}

void TelegramConfig::setPhoneNumber(const QString &phonenumber)
{
    this->updateStoragePath(this->_storagepath, phonenumber);
}

void TelegramConfig::setMe(User *me)
{
    if(this->_me == me)
        return;

    this->_me = me;
}

void TelegramConfig::saveDCConfig()
{
    QJsonArray jsonarray;

    foreach(DCConfig* dcconfig, this->_dcconfig.values())
        jsonarray.append(dcconfig->toJson());

    QJsonDocument doc(jsonarray);
    this->write(TelegramConfig::DCCONFIG_FILE, doc.toJson());
}

void TelegramConfig::loadDCConfig()
{
    if(!this->configExists(TelegramConfig::DCCONFIG_FILE))
        return;

    QString s = QString::fromUtf8(this->read(TelegramConfig::DCCONFIG_FILE));
    QJsonParseError parseerror;
    QJsonDocument docobj = QJsonDocument::fromJson(s.toUtf8(), &parseerror);

    if(parseerror.error != QJsonParseError::NoError)
    {
        qWarning() << "Invalid DC configuration, using defaults";
        return;
    }

    QJsonArray dcconfigarray = docobj.array();

    for(int i = 0; i < dcconfigarray.count(); i++)
    {
        QJsonObject jsonobj = dcconfigarray[i].toObject();
        DCConfig::Id id = DCConfig::configId(jsonobj);

        if(!id || !this->_dcconfig.contains(id))
        {
            if(!this->_dcconfig.contains(id))
                qWarning("Cannot find DC configuration %llx", id);
            else
                qWarning("Invalid DC configuration");

            continue;
        }

        this->_dcconfig[id]->fromJson(jsonobj);
    }
}

void TelegramConfig::saveConfig()
{
    QByteArray data;

    this->_config->serialize(data);
    this->write(TelegramConfig::CONFIG_FILE, data);
}

void TelegramConfig::loadConfig()
{
    if(!this->configExists(TelegramConfig::CONFIG_FILE))
        return;

    QByteArray data = this->read(TelegramConfig::CONFIG_FILE);

    Config* config = new Config();
    config->unserialize(data);

    this->setServerConfig(config);
}

void TelegramConfig::saveState()
{
    QByteArray data;

    this->_updatesstate->serialize(data);
    this->write(TelegramConfig::STATE_FILE, data);
}

void TelegramConfig::loadState()
{
    if(!this->configExists(TelegramConfig::STATE_FILE))
        return;

    QByteArray data = this->read(TelegramConfig::STATE_FILE);
    this->_updatesstate->unserialize(data);
}

void TelegramConfig::saveMe()
{
    if(!this->_me)
        return;

    QByteArray data;
    this->_me->serialize(data);
    this->write(TelegramConfig::ME_FILE, data);
}

void TelegramConfig::loadMe()
{
    if(!this->configExists(TelegramConfig::ME_FILE))
        return;

    if(!this->_me)
        this->_me = new User();

    QByteArray data = this->read(TelegramConfig::ME_FILE);
    this->_me->unserialize(data);
}

void TelegramConfig::updateStoragePath(const QString &storagepath, const QString &phonenumber)
{
    QDir storagepathdir(storagepath);

    this->_storagepath = storagepathdir.absoluteFilePath(phonenumber);
    this->_phonenumber = phonenumber;
}

void TelegramConfig::write(const QString &filename, const QByteArray& content)
{
    QDir dir(this->_storagepath);
    dir.mkpath(this->_storagepath);

    QFile f(dir.absoluteFilePath(filename));

    if(!f.open(QFile::WriteOnly))
    {
        qWarning() << "Cannot write" << dir.absoluteFilePath(filename);
        return;
    }

    f.write(content);
    f.close();
}

bool TelegramConfig::configExists(const QString &filename)
{
    QDir dir(this->_storagepath);
    return QFile::exists(dir.absoluteFilePath(filename));
}

QByteArray TelegramConfig::read(const QString &filename)
{
    QDir dir(this->_storagepath);
    dir.mkpath(this->_storagepath);

    QFile f(dir.absoluteFilePath(filename));

    if(!f.open(QFile::ReadOnly))
    {
        qWarning() << "Cannot read" << dir.absoluteFilePath(filename);
        return QByteArray();
    }

    QByteArray data = f.readAll();
    f.close();

    return data;
}

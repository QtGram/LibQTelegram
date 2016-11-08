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
const QString TelegramConfig::STATE_FILE = "state.mtproto";
const QString TelegramConfig::ME_FILE = "me.user";

TelegramConfig* TelegramConfig::_config = NULL;

TelegramConfig::TelegramConfig(): _debugmode(false), _ipv6(false), _layernum(0), _me(NULL)
{
    this->_storagepath = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QDir::separator() + CONFIG_FOLDER;
    this->_updatesstate = new UpdatesState();

    this->_devicemodel = QHostInfo::localHostName();

    if(this->_devicemodel.isEmpty())
        this->_devicemodel = "Unknown Device";
}

TelegramConfig *TelegramConfig::config()
{
    if(!TelegramConfig::_config)
        return NULL;

    return TelegramConfig::_config;
}

TelegramConfig *TelegramConfig::init(TLInt layernum, TLInt apiid, const QString &apihash, const QString &publickey, const QString& phonenumber)
{
    if(TelegramConfig::_config)
        return TelegramConfig::_config;

    TelegramConfig::_config = new TelegramConfig();
    TelegramConfig::_config->_layernum = layernum;
    TelegramConfig::_config->_apiid = apiid;
    TelegramConfig::_config->_apihash = apihash;
    TelegramConfig::_config->_publickey = publickey;
    TelegramConfig::_config->setPhoneNumber(phonenumber);
    TelegramConfig::_config->load();

    return TelegramConfig::_config;
}

int TelegramConfig::mainDcId() const
{
    if(this->_ipv6)
    {
        foreach(const DCConfig& dcconfig, this->_dcconfigipv6.values())
        {
            if(dcconfig.isMain())
                return dcconfig.id();
        }
    }
    else
    {
        foreach(const DCConfig& dcconfig, this->_dcconfig.values())
        {
            if(dcconfig.isMain())
                return dcconfig.id();
        }
    }

    return -1;
}

DCConfig &TelegramConfig::dcConfig(int dcid)
{
    if(this->_ipv6)
    {
        if(!this->_dcconfigipv6.contains(dcid))
            this->setDcConfig(dcid, true);

        return this->_dcconfigipv6[dcid];
    }

    if(!this->_dcconfig.contains(dcid))
        this->setDcConfig(dcid, false);

    return this->_dcconfig[dcid];
}

DCConfig &TelegramConfig::setDcConfig(int dcid, bool ipv6)
{
    if(ipv6)
    {
        if(!this->_dcconfigipv6.contains(dcid))
            this->_dcconfigipv6[dcid] = DCConfig(ipv6);

        return this->_dcconfigipv6[dcid];
    }

    if(!this->_dcconfig.contains(dcid))
        this->_dcconfig[dcid] = DCConfig(ipv6);

    return this->_dcconfig[dcid];
}

void TelegramConfig::setMainDc(int maindcid)
{
    QList<int> dcids;

    if(this->_ipv6)
        dcids = this->_dcconfigipv6.keys();
    else
        dcids = this->_dcconfig.keys();

    foreach(int dcid, dcids)
    {
        DCConfig& dcconfig = (this->_ipv6 ? this->_dcconfigipv6[dcid] : this->_dcconfig[dcid]);
        dcconfig.setIsMain(dcid == maindcid);
    }
}

UpdatesState *TelegramConfig::updateState()
{
    return this->_updatesstate;
}

User *TelegramConfig::me()
{
    return this->_me;
}

void TelegramConfig::save()
{
    this->saveDCConfig();
    this->saveState();
    this->saveMe();
}

void TelegramConfig::load()
{
    this->loadMe();
    this->loadState();
    this->loadDCConfig();
}

void TelegramConfig::reset()
{
    this->_dcconfig.clear();
    this->_dcconfigipv6.clear();
}

bool TelegramConfig::hasDC(int id)
{
    if(this->_ipv6)
        return this->_dcconfigipv6.contains(id);

    return this->_dcconfig.contains(id);
}

bool TelegramConfig::needsConfiguration() const
{
    if(this->_ipv6)
        return this->_dcconfigipv6.count() <= 1;

    return this->_dcconfig.count() <= 1;
}

bool TelegramConfig::isLoggedIn() const
{
    if(this->_ipv6)
    {
        foreach(const DCConfig& dcconfig, this->_dcconfigipv6.values())
        {
            if(dcconfig.authorization() == DCConfig::Signed)
                return true;
        }
    }
    else
    {
        foreach(const DCConfig& dcconfig, this->_dcconfig.values())
        {
            if(dcconfig.authorization() == DCConfig::Signed)
                return true;
        }
    }


    return false;
}

bool TelegramConfig::debugMode() const
{
    return this->_debugmode;
}

bool TelegramConfig::isIpv6() const
{
    return this->_ipv6;
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
    this->_ipv6 = ipv6;
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
    QJsonArray dcconfig;

    foreach(int dcid, this->_dcconfig.keys())
        dcconfig.append(this->_dcconfig[dcid].toJson());

    foreach(int dcid, this->_dcconfigipv6.keys())
        dcconfig.append(this->_dcconfigipv6[dcid].toJson());

    QJsonDocument doc(dcconfig);

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
        QJsonObject dcconfigobj = dcconfigarray[i].toObject();
        DCConfig& dcconfig = this->setDcConfig(dcconfigobj["id"].toInt(), dcconfigobj["ipv6"].toBool());

        if(!dcconfig.fromJson(dcconfigobj))
        {
            qWarning() << "Invalid DC configuration, using defaults";
            return;
        }
    }
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

#include "telegramconfig.h"
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QDebug>

#define CONFIG_FOLDER "telegram"

const QString TelegramConfig::DCCONFIG_FILE = "dcconfig.json";
TelegramConfig* TelegramConfig::_config = NULL;

TelegramConfig::TelegramConfig(): _debugmode(false), _ipv6(false), _layernum(0)
{
    this->_storagepath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QDir::separator() + CONFIG_FOLDER;

    this->_devicemodel = "Unknown Device";
    this->_osversion = "Unknown OS";
    this->_appversion = "1.0";
}

TelegramConfig *TelegramConfig::config()
{
    if(!TelegramConfig::_config)
    {
        qFatal("NO CONFIGURATION SET");
        return NULL;
    }

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

void TelegramConfig::save()
{
    this->saveDCConfig();
}

void TelegramConfig::load()
{
    this->loadDCConfig();
}

bool TelegramConfig::hasDC(int id)
{
    if(this->_ipv6)
        return this->_dcconfigipv6.contains(id);

    return this->_dcconfig.contains(id);
}

bool TelegramConfig::debugMode() const
{
    return this->_debugmode;
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

const QString &TelegramConfig::osVersion() const
{
    return this->_osversion;
}

const QString &TelegramConfig::applicationVersion() const
{
    return this->_appversion;
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

void TelegramConfig::setApplicationVersion(const QString &appversion)
{
    this->_appversion = appversion;
}

void TelegramConfig::setPhoneNumber(const QString &phonenumber)
{
    this->updateStoragePath(this->_storagepath, phonenumber);
}

void TelegramConfig::saveDCConfig()
{
    QJsonArray dcconfig;

    foreach(int dcid, this->_dcconfig.keys())
        dcconfig.append(this->_dcconfig[dcid].toJson());

    foreach(int dcid, this->_dcconfigipv6.keys())
        dcconfig.append(this->_dcconfigipv6[dcid].toJson());

    QJsonDocument doc(dcconfig);

    this->write(TelegramConfig::DCCONFIG_FILE, QString(doc.toJson()));
}

void TelegramConfig::loadDCConfig()
{
    if(!this->configExists(TelegramConfig::DCCONFIG_FILE))
        return;

    QString s = this->read(TelegramConfig::DCCONFIG_FILE);
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

void TelegramConfig::updateStoragePath(const QString &storagepath, const QString &phonenumber)
{
    QDir storagepathdir(storagepath);

    this->_storagepath = storagepathdir.absoluteFilePath(phonenumber);
    this->_phonenumber = phonenumber;
}

void TelegramConfig::write(const QString &filename, const QString& content)
{
    QDir dir(this->_storagepath);
    dir.mkpath(this->_storagepath);

    QFile f(dir.absoluteFilePath(filename));

    if(!f.open(QFile::WriteOnly))
    {
        qWarning() << "Cannot write" << dir.absoluteFilePath(filename);
        return;
    }

    f.write(content.toUtf8());
    f.close();
}

bool TelegramConfig::configExists(const QString &filename)
{
    QDir dir(this->_storagepath);
    return QFile::exists(dir.absoluteFilePath(filename));
}

QString TelegramConfig::read(const QString &filename)
{
    QDir dir(this->_storagepath);
    dir.mkpath(this->_storagepath);

    QFile f(dir.absoluteFilePath(filename));

    if(!f.open(QFile::ReadOnly))
    {
        qWarning() << "Cannot read" << dir.absoluteFilePath(filename);
        return QString();
    }

    QString s = QString(f.readAll());
    f.close();

    return s;
}

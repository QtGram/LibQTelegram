#include "dcconfig.h"
#include "../crypto/hash.h"
#include <QJsonObject>

DCConfig::DCConfig(const QString &host, TLInt port, TLInt dcid, bool isipv6, QObject *parent): QObject(parent)
{
    this->_authorization = DCConfig::NotAuthorized;
    this->_authorizationkeyid = 0;
    this->_serversalt = 0;
    this->_deltatime = 0;
    this->_ismain = false;

    // Handcraft a DcOption
    this->_dcoption = new DcOption(this);
    this->_dcoption->setId(dcid);
    this->_dcoption->setIsIpv6(isipv6);
    this->_dcoption->setIpAddress(ToTLString(host));
    this->_dcoption->setPort(port);

    this->_id = MakeDCConfigId(this->_dcoption->isIpv6(), this->_dcoption->isMediaOnly(), this->_dcoption->id());
}

DCConfig::DCConfig(DcOption *dcoption, QObject *parent): QObject(parent), _dcoption(dcoption)
{
    this->_authorization = DCConfig::NotAuthorized;
    this->_authorizationkeyid = 0;
    this->_serversalt = 0;
    this->_deltatime = 0;
    this->_ismain = false;
    this->_id = MakeDCConfigId(dcoption->isIpv6(), dcoption->isMediaOnly(), dcoption->id());
}

bool DCConfig::isMain() const
{
    return this->_ismain;
}

QJsonObject DCConfig::toJson() const
{
    QJsonObject jsonobj;

    jsonobj["id"] = QString::number(this->_id, 16);
    jsonobj["deltatime"] = this->_deltatime;
    jsonobj["serversalt"] = QString::number(this->_serversalt, 16);
    jsonobj["authorization"] = this->_authorization;
    jsonobj["authorizationkey"] = QString(this->_authorizationkey.toHex());
    jsonobj["main"] = this->_ismain;

    return jsonobj;
}

void DCConfig::fromJson(const QJsonObject& jsonobj)
{
    if(jsonobj.contains("id"))
        this->_id = DCConfig::configId(jsonobj);

    if(jsonobj.contains("deltatime"))
        this->_deltatime = jsonobj["deltatime"].toInt();

    if(jsonobj.contains("serversalt"))
        this->_serversalt = jsonobj["serversalt"].toString().toULongLong(NULL, 16);

    if(jsonobj.contains("authorization"))
        this->_authorization = jsonobj["authorization"].toInt();

    if(jsonobj.contains("authorizationkey"))
        this->_authorizationkey = QByteArray::fromHex(jsonobj["authorizationkey"].toString().toUtf8());

    if(jsonobj.contains("main"))
        this->_ismain = jsonobj["main"].toBool();
}

void DCConfig::reset()
{
    this->_authorization = DCConfig::NotAuthorized;
    this->_authorizationkey.clear();
    this->_authorizationkeyauxhash.clear();

    this->_authorizationkeyid = 0;
    this->_deltatime = 0;
    this->_serversalt = 0;
}

DCConfig::Id DCConfig::configId(const QJsonObject &jsonobj)
{
    if(!jsonobj.contains("id"))
        return 0;

    return jsonobj["id"].toString().toULongLong(NULL, 16);
}

void DCConfig::setDcOption(DcOption *dcoption)
{
    if(this->_dcoption && this->_dcoption->parent() == this)
        this->_dcoption->deleteLater();

    this->_dcoption = dcoption;
}

void DCConfig::setAuthorization(int authorization)
{
    this->_authorization = authorization;
}

DCConfig::Id DCConfig::id() const
{
    return this->_id;
}

DcOption *DCConfig::option() const
{
    return this->_dcoption;
}

TLInt DCConfig::dcid() const
{
    return this->_dcoption->id();
}

QString DCConfig::host() const
{
    return this->_dcoption->ipAddress();
}

qint16 DCConfig::port() const
{
    return this->_dcoption->port();
}

int DCConfig::authorization() const
{
    return this->_authorization;
}

void DCConfig::setDeltaTime(TLInt deltatime)
{
    this->_deltatime = deltatime;
}

void DCConfig::setIsMain(bool b)
{
    this->_ismain = b;
}

void DCConfig::setServerSalt(TLLong serversalt)
{
    this->_serversalt = serversalt;
}

void DCConfig::setAuthorizationKey(const QByteArray &authorizationkey)
{
    this->_authorizationkey = authorizationkey;
}

const QByteArray &DCConfig::authorizationKey() const
{
    return this->_authorizationkey;
}

const QByteArray &DCConfig::authorizationKeyAuxHash()
{
    if(this->_authorizationkeyauxhash.isEmpty())
        this->_authorizationkeyauxhash = sha1_hash(this->_authorizationkey).left(8);

    return this->_authorizationkeyauxhash;
}

TLLong DCConfig::authorizationKeyId()
{
    if(!this->_authorizationkeyid)
    {
        QByteArray lowauthkey = sha1_hash(this->_authorizationkey).mid(12);
        this->_authorizationkeyid = *(reinterpret_cast<const TLLong*>(lowauthkey.constData()));
    }

    return this->_authorizationkeyid;
}

TLLong DCConfig::serverSalt() const
{
    return this->_serversalt;
}

TLLong DCConfig::deltaTime() const
{
    return this->_deltatime;
}

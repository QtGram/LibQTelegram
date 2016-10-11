#include "dcconfig.h"
#include "../crypto/sha1.h"
#include <QJsonObject>
#include <QDateTime>

DCConfig::DCConfig(bool ipv6): _port(0), _deltatime(0), _authorizationkeyid(0), _serversalt(0), _authorization(DCConfig::NotAuthorized), _id(0), _ipv6(ipv6), _ismain(false)
{

}

DCConfig::DCConfig(const DCConfig &dcconfig)
{
    this->_host = dcconfig._host;
    this->_port = dcconfig._port;
    this->_deltatime = dcconfig._deltatime;
    this->_authorizationkey = dcconfig._authorizationkey;
    this->_authorizationkeyauxhash = dcconfig._authorizationkeyauxhash;
    this->_authorizationkeyid = dcconfig._authorizationkeyid;
    this->_serversalt = dcconfig._serversalt;
    this->_authorization = dcconfig._authorization;
    this->_id = dcconfig._id;
    this->_ipv6 = dcconfig._ipv6;
    this->_ismain = dcconfig._ismain;
}

QJsonObject DCConfig::toJson()
{
    QJsonObject jsonobj;

    jsonobj["id"] = this->_id;
    jsonobj["host"] = this->_host;
    jsonobj["port"] = this->_port;
    jsonobj["deltatime"] = this->_deltatime;
    jsonobj["serversalt"] = this->_serversalt;
    jsonobj["authorization"] = this->_authorization;
    jsonobj["authorizationkey"] = QString(this->_authorizationkey.toHex());
    jsonobj["ipv6"] = this->_ipv6;
    jsonobj["main"] = this->_ismain;

    return jsonobj;
}

bool DCConfig::fromJson(const QJsonObject &jsonobj)
{
    if(jsonobj.contains("id"))
        this->_id = jsonobj["id"].toInt();

    if(jsonobj.contains("host"))
        this->_host = jsonobj["host"].toString();

    if(jsonobj.contains("port"))
        this->_port = jsonobj["port"].toInt();

    if(jsonobj.contains("deltatime"))
        this->_deltatime = jsonobj["deltatime"].toInt();

    if(jsonobj.contains("serversalt"))
        this->_serversalt = jsonobj["serversalt"].toInt();

    if(jsonobj.contains("authorization"))
        this->_authorization = jsonobj["authorization"].toInt();

    if(jsonobj.contains("authorizationkey"))
        this->_authorizationkey = QByteArray::fromHex(jsonobj["authorizationkey"].toString().toUtf8());

    if(jsonobj.contains("ipv6"))
        this->_ipv6 = jsonobj["ipv6"].toBool();

    if(jsonobj.contains("main"))
        this->_ismain = jsonobj["main"].toBool();

    return true;
}

const QString &DCConfig::host() const
{
    return this->_host;
}

qint16 DCConfig::port() const
{
    return this->_port;
}

TLInt DCConfig::serverTime() const
{
    return this->_deltatime;
}

const QByteArray &DCConfig::authorizationKey() const
{
    return this->_authorizationkey;
}

QByteArray DCConfig::authorizationKeyAuxHash()
{
    if(this->_authorizationkeyauxhash.isEmpty())
        this->_authorizationkeyauxhash = Sha1::hash(this->_authorizationkey).left(8);

    return this->_authorizationkeyauxhash;
}

TLLong DCConfig::authorizationKeyId()
{
    if(!this->_authorizationkeyid)
    {
        QByteArray lowauthkey = Sha1::hash(this->_authorizationkey).mid(12);
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

int DCConfig::authorization() const
{
    return this->_authorization;
}

int DCConfig::id() const
{
    return this->_id;
}

bool DCConfig::isMain() const
{
    return this->_ismain;
}

void DCConfig::setHost(const QString &host)
{
    this->_host = host;
}

void DCConfig::setPort(qint16 port)
{
    this->_port = port;
}

void DCConfig::setDeltaTime(TLInt deltatime)
{
    this->_deltatime = deltatime;
}

void DCConfig::setAuthorizationKey(const QByteArray &authorizationkey)
{
    this->_authorizationkey = authorizationkey;
}

void DCConfig::setAuthorization(int authorization)
{
    this->_authorization = authorization;
}

void DCConfig::setServerSalt(TLLong serversalt)
{
    this->_serversalt = serversalt;
}

void DCConfig::setId(int id)
{
    this->_id = id;
}

void DCConfig::setIpv6(bool b)
{
    this->_ipv6 = b;
}

void DCConfig::setIsMain(bool b)
{
    this->_ismain = b;
}

void DCConfig::reset()
{
    this->_authorization = DCConfig::NotAuthorized;
    this->_authorizationkey.clear();
    this->_authorizationkeyauxhash.clear();

    this->_authorizationkeyid = 0;
    this->_deltatime = 0;
    this->_serversalt = 0;
    this->_ipv6 = false;
}

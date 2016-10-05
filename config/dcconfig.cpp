#include "dcconfig.h"
#include "../crypto/sha1.h"
#include <QJsonObject>
#include <QDateTime>

DCConfig::DCConfig(bool ipv6): _port(0), _servertime(0), _authorizationkeyid(0), _serversalt(0), _authorization(DCConfig::NotAuthorized), _id(0), _ipv6(ipv6)
{

}

DCConfig::DCConfig(const DCConfig &dcconfig)
{
    this->_host = dcconfig._host;
    this->_port = dcconfig._port;
    this->_servertime = dcconfig._servertime;
    this->_authorizationkey = dcconfig._authorizationkey;
    this->_authorizationkeyauxhash = dcconfig._authorizationkeyauxhash;
    this->_authorizationkeyid = dcconfig._authorizationkeyid;
    this->_serversalt = dcconfig._serversalt;
    this->_authorization = dcconfig._authorization;
    this->_id = dcconfig._id;
    this->_ipv6 = dcconfig._ipv6;
}

QJsonObject DCConfig::toJson()
{
    QJsonObject jsonobj;

    jsonobj["id"] = this->_id;
    jsonobj["host"] = this->_host;
    jsonobj["port"] = this->_port;
    jsonobj["servertime"] = this->_servertime;
    jsonobj["serversalt"] = this->_serversalt;
    jsonobj["authorizationkey"] = QString(this->_authorizationkey.toHex());
    jsonobj["ipv6"] = this->_ipv6 ? "true" : "false";

    return jsonobj;
}

bool DCConfig::fromJson(const QJsonObject &jsonobj)
{
    if(!jsonobj.contains("host"))
        return false;

    this->_host = jsonobj["host"].toString();

    if(!jsonobj.contains("port"))
        return false;

    this->_port = jsonobj["port"].toInt();

    if(!jsonobj.contains("servertime"))
        return false;

    this->_servertime = jsonobj["servertime"].toInt();

    if(!jsonobj.contains("serversalt"))
        return false;

    this->_servertime = jsonobj["serversalt"].toInt();

    if(!jsonobj.contains("authorizationkey"))
        return false;

    QByteArray authkeyhex = jsonobj["serversalt"].toString().toUtf8();
    this->_authorizationkey = QByteArray::fromHex(authkeyhex);

    if(!this->_authorizationkey.isEmpty())
        this->_authorization = DCConfig::Authorized;
    else
        this->_authorization = DCConfig::NotAuthorized;

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
    return this->_servertime;
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
    if(!this->_servertime)
        return 0;

    return this->_servertime - QDateTime::currentDateTime().toTime_t();
}

int DCConfig::authorization() const
{
    return this->_authorization;
}

int DCConfig::id() const
{
    return this->_id;
}

void DCConfig::setHost(const QString &host)
{
    this->_host = host;
}

void DCConfig::setPort(qint16 port)
{
    this->_port = port;
}

void DCConfig::setServerTime(TLInt servertime)
{
    this->_servertime = servertime;
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

#ifndef DCCONFIG_H
#define DCCONFIG_H

#include <QObject>
#include "../types/basic.h"

class DCConfig
{
    public:
        enum DcAuthorization {
            NotAuthorized = 0,
            PQReceived,
            ServerDHParamsFailReceived,
            ServerDHParamsOkReceived,
            Authorized,
            Signed,
        };

    public:
        DCConfig(bool ipv6 = false);
        DCConfig(const DCConfig& dcconfig);
        QJsonObject toJson();
        bool fromJson(const QJsonObject& jsonobj);
        const QString& host() const;
        qint16 port() const;
        TLInt serverTime() const;
        const QByteArray& authorizationKey() const;
        QByteArray authorizationKeyAuxHash();
        TLLong authorizationKeyId();
        TLLong serverSalt() const;
        TLLong deltaTime() const;
        int authorization() const;
        int id() const;
        bool isMain() const;
        void setHost(const QString& host);
        void setPort(qint16 port);
        void setDeltaTime(TLInt deltatime);
        void setAuthorizationKey(const QByteArray& authorizationkey);
        void setAuthorization(int authorization);
        void setServerSalt(TLLong serversalt);
        void setId(int id);
        void setIpv6(bool b);
        void setIsMain(bool b);
        void reset();

    private:
        QString _host;
        qint16 _port;
        TLInt _deltatime;
        QByteArray _authorizationkey;
        QByteArray _authorizationkeyauxhash;
        TLLong _authorizationkeyid;
        TLLong _serversalt;
        int _authorization;
        int _id;
        bool _ipv6;
        bool _ismain;
};

#endif // DCCONFIG_H

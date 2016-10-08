#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <QObject>
#include "libqtelegram_global.h"
#include "mtproto/dc/dcsessionmanager.h"
#include "autogenerated/telegramapi.h"

class LIBQTELEGRAMSHARED_EXPORT Telegram : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString publicKey READ publicKey WRITE setPublicKey NOTIFY publicKeyChanged)
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(QString phoneNumber READ phoneNumber WRITE setPhoneNumber NOTIFY phoneNumberChanged)
    Q_PROPERTY(QString apiHash READ apiHash WRITE setApiHash NOTIFY apiHashChanged)
    Q_PROPERTY(qint32 apiId READ apiId WRITE setApiId NOTIFY apiIdChanged)
    Q_PROPERTY(qint32 port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(qint32 dcId READ dcId WRITE setDcId NOTIFY dcIdChanged)
    Q_PROPERTY(bool debugMode READ debugMode WRITE setDebugMode NOTIFY debugModeChanged)

    public:
        explicit Telegram(QObject *parent = 0);
        ~Telegram();
        const QString& publicKey() const;
        const QString& host() const;
        const QString& apiHash() const;
        const QString& phoneNumber() const;
        qint32 apiId() const;
        qint32 port() const;
        qint32 dcId() const;
        bool debugMode() const;

    public:
        void setPublicKey(const QString& publickey);
        void setHost(const QString& host);
        void setApiHash(const QString& apphash);
        void setPhoneNumber(const QString& phonenumber);
        void setApiId(qint32 appid);
        void setPort(qint32 port);
        void setDcId(qint32 dcid);
        void setDebugMode(bool dbgmode);

    public slots:
        void signIn(const QString& phonecode);
        void signUp(const QString& firstname, const QString& lastname, const QString& phonecode);

    private:
        void tryConnect();

    private slots:
        void onAuthCheckPhoneReplied(MTProtoReply *mtreply);
        void onLoginCompleted(MTProtoReply* mtreply);

    signals:
        void signUpRequested();
        void signInRequested();
        void loginCompleted();
        void publicKeyChanged();
        void hostChanged();
        void apiHashChanged();
        void phoneNumberChanged();
        void apiIdChanged();
        void portChanged();
        void dcIdChanged();
        void debugModeChanged();

    private:
        QString _publickey;
        QString _host;
        QString _phonenumber;
        QString _apihash;
        qint32 _apiid;
        qint32 _port;
        qint32 _dcid;
        bool _debugmode;

     private:
        TLString _phonecodehash;
};

#endif // TELEGRAM_H

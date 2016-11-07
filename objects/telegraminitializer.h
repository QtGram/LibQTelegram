#ifndef TELEGRAMINITIALIZER_H
#define TELEGRAMINITIALIZER_H

#include <QObject>
#include "libqtelegram_global.h"
#include "mtproto/dc/dcsessionmanager.h"
#include "autogenerated/telegramapi.h"

class TelegramInitializer : public QObject
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
        explicit TelegramInitializer(QObject *parent = 0);
        ~TelegramInitializer();
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

    public:
        void signIn(const QString& phonecode) const;
        void signUp(const QString& firstname, const QString& lastname, const QString& phonecode) const;
        void sendPassword(const QString& password) const;
        void resendCode() const;

    private:
        void tryConnect();

    protected:
        virtual void timerEvent(QTimerEvent *event);

    private slots:
        void onMainSessionReady(DCSession *dcsession);
        void onAuthCheckPhoneReplied(MTProtoReply *mtreply);
        void onLoginCompleted(MTProtoReply* mtreply);
        void onAccountGetPasswordReplied(MTProtoReply* mtreply);
        void onFloodWait(int seconds);
        void onSessionPasswordNeeded();

    signals:
        void floodWait(int seconds);
        void phoneCodeError(QString errormessage);
        void sessionPasswordNeeded(QString hint);
        void invalidPassword();
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
        int _dcid;
        bool _debugmode;

     private:
        AccountPassword* _accountpassword;
        TLString _phonecodehash;
        int _floodwaittimer;
};

#endif // TELEGRAM_H

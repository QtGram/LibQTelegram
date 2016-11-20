#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <QObject>
#include "objects/telegraminitializer.h"

class Telegram : public QObject
{
    Q_OBJECT

    Q_PROPERTY(TelegramInitializer* initializer READ initializer WRITE setInitializer NOTIFY initializerChanged)
    Q_PROPERTY(User* me READ me CONSTANT FINAL)
    Q_PROPERTY(int apiLayer READ apiLayer CONSTANT FINAL)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(bool syncing READ syncing NOTIFY syncingChanged)
    Q_PROPERTY(bool autoDownload READ autoDownload WRITE setAutoDownload NOTIFY autoDownloadChanged)

    public:
        explicit Telegram(QObject *parent = 0);
        ~Telegram();
        TelegramInitializer* initializer() const;
        User* me() const;
        int apiLayer() const;
        bool connected() const;
        bool syncing() const;
        bool autoDownload() const;
        void setInitializer(TelegramInitializer* initializer);
        void setAutoDownload(bool b);

    public: // C++ side API
        bool muteDialog(Dialog* dialog, bool mute);
        QString dialogTitle(Dialog* dialog) const;
        QString dialogStatusText(Dialog* dialog) const;
        QString messagePreview(Message* message) const;
        QString messageText(Message* message) const;
        TelegramObject* messageFrom(Message *message) const;

    public slots: // Login
        void signIn(const QString& phonecode) const;
        void signUp(const QString& firstname, const QString& lastname, const QString& phonecode) const;
        void sendPassword(const QString& password) const;
        void resendCode() const;

    private slots:
        void onConfigurationReady();

    private:
        QString userList(const TLVector<TLInt> users) const;
        QString messageMediaText(MessageMedia* messagemedia) const;
        QString messageActionText(Message *message) const;

    signals:
        void initializerChanged();
        void syncingChanged();
        void autoDownloadChanged();
        void connectedChanged();
        void signUpRequested();
        void signInRequested();
        void loginCompleted();
        void invalidPassword();
        void floodLock(int seconds);
        void sessionPasswordNeeded(QString hint);
        void phoneCodeError(QString errormessage);

    private:
        TelegramInitializer* _initializer;
        bool _autodownload;
};

#endif // TELEGRAM_H

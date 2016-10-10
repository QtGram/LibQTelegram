#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <QObject>
#include "objects/telegraminitializer.h"
#include "objects/fileobject.h"

class Telegram : public QObject
{
    Q_OBJECT

    Q_PROPERTY(TelegramInitializer* initializer READ initializer WRITE setInitializer NOTIFY initializerChanged)
    Q_PROPERTY(User* me READ me CONSTANT FINAL)

    public:
        explicit Telegram(QObject *parent = 0);
        TelegramInitializer* initializer() const;
        User* me() const;
        void setInitializer(TelegramInitializer* initializer);

    public slots: // Login
        void signIn(const QString& phonecode);
        void signUp(const QString& firstname, const QString& lastname, const QString& phonecode);

    public slots: // Dialogs
        QString dialogTitle(Dialog* dialog);
        QString dialogDraftMessage(Dialog* dialog);
        bool dialogHasDraftMessage(Dialog* dialog);

    public slots: // Users
        QString userFullName(User* user);
        QString userStatusText(User* user);

    public slots: // Messages
        QString messageText(TLInt messageid);

    public slots:
        FileObject* fileObject(Dialog* dialog);

    signals:
        void initializerChanged();
        void signUpRequested();
        void signInRequested();
        void loginCompleted();

    private:
        TelegramInitializer* _initializer;
};

#endif // TELEGRAM_H

#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <QObject>
#include "objects/telegraminitializer.h"

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

    public slots:
        QString dialogTitle(Dialog* dialog);
        QString userFullName(User* user);
        QString userStatusText(User* user);
        QString messageText(TLInt messageid);
        void signIn(const QString& phonecode);
        void signUp(const QString& firstname, const QString& lastname, const QString& phonecode);

    signals:
        void initializerChanged();
        void signUpRequested();
        void signInRequested();
        void loginCompleted();

    private:
        TelegramInitializer* _initializer;
};

#endif // TELEGRAM_H

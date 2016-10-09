#ifndef TELEGRAM_H
#define TELEGRAM_H

#include <QObject>
#include "objects/telegraminitializer.h"

class Telegram : public QObject
{
    Q_OBJECT

    Q_PROPERTY(TelegramInitializer* initializer READ initializer WRITE setInitializer NOTIFY initializerChanged)

    public:
        explicit Telegram(QObject *parent = 0);
        TelegramInitializer* initializer() const;
        void setInitializer(TelegramInitializer* initializer);

    public slots:
        QString dialogTitle(Dialog* dialog);
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

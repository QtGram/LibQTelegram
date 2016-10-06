#ifndef TELEGRAMINITIALIZER_H
#define TELEGRAMINITIALIZER_H

#include <QObject>
#include <QString>

class TelegramInitializer
{
    private:
        TelegramInitializer();

    public:
        static void initialize(const QString& uri, const QString& prefix = QString());
};

#endif // TELEGRAMINITIALIZER_H

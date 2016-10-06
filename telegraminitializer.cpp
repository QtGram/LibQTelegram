#include "telegraminitializer.h"
#include "telegram.h"
#include <QtQml>

#define TG_VERSION_MAJOR 1
#define TG_VERSION_MINOR 0

#define TG_IMPORT(uri) uri.toUtf8().constData()
#define TG_NAME(prefix, name) QString("%1%2").arg(prefix, name).toUtf8().constData()

TelegramInitializer::TelegramInitializer()
{

}

void TelegramInitializer::initialize(const QString &uri, const QString &prefix)
{
    qmlRegisterType<Telegram>(TG_IMPORT(uri), TG_VERSION_MAJOR, TG_VERSION_MINOR, TG_NAME(prefix, "Telegram"));
}

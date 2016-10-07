#include "telegraminitializer.h"
#include "telegram.h"
#include "models/dialogsmodel.h"
#include "objects/objects.h"
#include <QtQml>

#define TG_VERSION_MAJOR 1
#define TG_VERSION_MINOR 0

#define TG_IMPORT(uri) uri.toUtf8().constData()
#define TG_NAME(prefix, name) QString("%1%2").arg(prefix, name).toUtf8().constData()

#define REGISTER_TYPE(type) \
    qRegisterMetaType<type>();

#define REGISTER_OBJECT(objectname) \
    qmlRegisterType<objectname>(TG_IMPORT(uri), TG_VERSION_MAJOR, TG_VERSION_MINOR, TG_NAME(prefix, #objectname));

TelegramInitializer::TelegramInitializer()
{

}

void TelegramInitializer::initialize(const QString &uri, const QString &prefix)
{
    REGISTER_TYPE(TLTrue)
    REGISTER_TYPE(TLBool)
    REGISTER_TYPE(TLInt)

    REGISTER_OBJECT(User);
    REGISTER_OBJECT(Chat);

    REGISTER_OBJECT(Telegram);
    REGISTER_OBJECT(DialogObject);

    REGISTER_OBJECT(DialogsModel);
}

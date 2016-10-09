#include "telegramqmlbase.h"
#include "telegram.h"
#include "telegraminitializer.h"
#include "models/dialogsmodel.h"

TelegramQmlBase::TelegramQmlBase()
{

}

void TelegramQmlBase::initialize(const QString &uri)
{
    // Basic types
    REGISTER_TYPE(TLTrue);
    REGISTER_TYPE(TLBool);
    REGISTER_TYPE(TLInt);
    REGISTER_TYPE(TLBytes);
    REGISTER_TYPE(TLString);

    // Main objects
    REGISTER_OBJECT(TelegramInitializer);
    REGISTER_OBJECT(Telegram);

    // Models
    REGISTER_OBJECT(DialogsModel);
}

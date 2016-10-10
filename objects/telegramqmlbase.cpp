#include "telegramqmlbase.h"
#include "telegram.h"
#include "telegraminitializer.h"
#include "objects/fileobject.h"
#include "models/dialogsmodel.h"
#include "models/contactsmodel.h"

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
    REGISTER_TYPE(TLConstructor);

    // Main objects
    REGISTER_OBJECT(TelegramInitializer);
    REGISTER_OBJECT(Telegram);

    // Extra objects
    REGISTER_OBJECT(FileObject);

    // Models
    REGISTER_OBJECT(DialogsModel);
    REGISTER_OBJECT(ContactsModel);
}

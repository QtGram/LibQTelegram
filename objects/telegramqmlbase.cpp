#include "telegramqmlbase.h"
#include "telegram.h"
#include "telegraminitializer.h"
#include "models/dialogsmodel.h"
#include "models/messagesmodel.h"
#include "models/contactsmodel.h"
#include "quick/qquickpeerimage.h"
#include "quick/qquickmediamessageitem.h"

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

    // Models
    REGISTER_OBJECT(ContactsModel);
    REGISTER_OBJECT(DialogsModel);
    REGISTER_OBJECT(MessagesModel);

    // Components
    REGISTER_OBJECT_AS(QQuickPeerImage, "PeerImage");
    REGISTER_OBJECT_AS(QQuickMediaMessageItem, "MediaMessageItem");
}

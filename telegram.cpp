#include "telegram.h"
#include "config/cache/telegramcache.h"
#include "config/cache/filecache.h"
#include "types/telegramhelper.h"

Telegram::Telegram(QObject *parent) : QObject(parent), _initializer(NULL)
{
}

TelegramInitializer *Telegram::initializer() const
{
    return this->_initializer;
}

User *Telegram::me() const
{
    return TelegramConfig_instance->me();
}

void Telegram::setInitializer(TelegramInitializer *initializer)
{
    if(this->_initializer == initializer)
        return;

    if(this->_initializer)
    {
        disconnect(this->_initializer, &TelegramInitializer::signUpRequested, this, 0);
        disconnect(this->_initializer, &TelegramInitializer::signInRequested, this, 0);
        disconnect(this->_initializer, &TelegramInitializer::loginCompleted, this, 0);
    }

    this->_initializer = initializer;

    connect(this->_initializer, &TelegramInitializer::signUpRequested, this, &Telegram::signUpRequested);
    connect(this->_initializer, &TelegramInitializer::signInRequested, this, &Telegram::signInRequested);
    connect(this->_initializer, &TelegramInitializer::loginCompleted, this, &Telegram::loginCompleted);

    emit initializerChanged();
}

QString Telegram::messageMediaText(MessageMedia *messagemedia)
{
    TLConstructor ctorid = messagemedia->constructorId();

    if(ctorid == TLTypes::MessageMediaWebPage)
    {
        if(!messagemedia->caption().isEmpty())
            return  messagemedia->caption();

        return messagemedia->webpage()->url();
    }

    if(ctorid == TLTypes::MessageMediaDocument)
    {
        Document* document = messagemedia->document();

        foreach(DocumentAttribute* attribute, document->attributes())
        {
            if(attribute->constructorId() == TLTypes::DocumentAttributeImageSize)
                return tr("Photo");

            if(attribute->constructorId() == TLTypes::DocumentAttributeAnimated)
                return tr("GIF");

            if(attribute->constructorId() == TLTypes::DocumentAttributeSticker)
                return tr("Sticker");

            if(attribute->constructorId() == TLTypes::DocumentAttributeVideo)
                return tr("Video");

            if(attribute->constructorId() == TLTypes::DocumentAttributeAudio)
                return tr("Audio recording");

            if(attribute->constructorId() == TLTypes::DocumentAttributeHasStickers)
                return tr("Sticker set");
        }

        return tr("Document");
    }

    QString result;

    if(ctorid == TLTypes::MessageMediaContact)
        result = tr("Contact");
    else if(ctorid == TLTypes::MessageMediaGame)
        result = tr("Game");
    else if((ctorid == TLTypes::MessageMediaGeo)  || (ctorid == TLTypes::MessageMediaVenue))
        result = tr("Location");
    else if(ctorid == TLTypes::MessageMediaPhoto)
        result = tr("Photo");

    if(!messagemedia->caption().isEmpty())
        result += ", " + messagemedia->caption();

    return result;
}

QString Telegram::messageActionText(MessageAction *messageaction)
{
    User* inviteruser = NULL, *user = NULL;
    QString fullname = "???", inviterfullname = "???";
    TLConstructor ctorid = messageaction->constructorId();

    if(messageaction->userId())
    {
        user = TelegramCache_user(messageaction->userId());

        if(user)
            fullname = TelegramHelper::fullName(user);
    }

    if(messageaction->inviterId())
    {
        inviteruser = TelegramCache_user(messageaction->inviterId());

        if(inviteruser)
            inviterfullname = TelegramHelper::fullName(inviteruser);
    }

    if(ctorid == TLTypes::MessageActionChatCreate)
    {
        if(messageaction->channelId())
            return tr("Channel created by «%1»").arg(fullname);

        return tr("Group created by «%1»").arg(fullname);
    }

    if(ctorid == TLTypes::MessageActionChatEditTitle)
        return tr("Group name changed to «%1»").arg(messageaction->title().toString());

    if(ctorid == TLTypes::MessageActionChatEditPhoto)
        return tr("«%1» updated group photo").arg(fullname);

    if(ctorid == TLTypes::MessageActionChatDeletePhoto)
        return tr("«%1» deleted group photo").arg(fullname);

    if(ctorid == TLTypes::MessageActionChatDeletePhoto)
        return tr("«%1» deleted group photo").arg(fullname);

    if(ctorid == TLTypes::MessageActionChatAddUser)
    {
        if(inviteruser)
            return tr("«%1» added «%2» ").arg(inviterfullname, fullname);

        return tr("«%1» has joined the group").arg(fullname);
    }

    if(ctorid == TLTypes::MessageActionChatDeleteUser)
        return tr("«%1» has left the group").arg(fullname);

    if(ctorid == TLTypes::MessageActionChatJoinedByLink)
        return tr("«%1» has joined the group via invite link").arg(fullname);

    if(ctorid == TLTypes::MessageActionChannelCreate)
        return tr("Channel «%1» created").arg(messageaction->title().toString());

    if(ctorid == TLTypes::MessageActionChatMigrateTo)
    {
        if(messageaction->channelId())
            return tr("This group was upgraded to a supergroup");

        return tr("Unhandled chat migration type");
    }

    if(ctorid == TLTypes::MessageActionChannelMigrateFrom) // NOTE: MessageActionChannelMigrateFrom, untested
    {
        if(messageaction->channelId())
            return tr("This supergroup was downgraded to a group");

        return tr("Unhandled channel migration type");
    }

    if(ctorid == TLTypes::MessageActionGameScore) // MessageActionGameScore, unhandled and untested
        return tr("Unhandled MessageActionGameScore");

    return QString("Unhandled action: %1").arg(ctorid, 0, 16);
}

QString Telegram::dialogTitle(Dialog *dialog)
{
    TLInt peerid = TelegramHelper::identifier(dialog->peer());

    if(TelegramHelper::isChat(dialog) || TelegramHelper::isChannel(dialog))
    {
        Chat* chat = TelegramCache_chat(peerid);
        return chat->title();
    }

    User* user = TelegramCache_user(peerid);
    return TelegramHelper::fullName(user);
}

QString Telegram::dialogDraftMessage(Dialog *dialog)
{
    if(dialog->draft())
        return dialog->draft()->message().toString();

    return QString();
}

bool Telegram::dialogIsChat(Dialog *dialog)
{
    return TelegramHelper::isChat(dialog);
}

bool Telegram::dialogIsChannel(Dialog *dialog)
{
    return TelegramHelper::isChannel(dialog);
}

bool Telegram::dialogIsBroadcast(Dialog *dialog)
{
    if(!TelegramHelper::isChannel(dialog))
        return false;

    Chat* chat = TelegramCache_chat(TelegramHelper::identifier(dialog->peer()));

    if(chat)
        return chat->isBroadcast();

    return false;
}

bool Telegram::dialogHasDraftMessage(Dialog *dialog)
{
    return dialog->draft() && (dialog->draft()->constructorId() != DraftMessage::CtorDraftMessageEmpty);
}

QString Telegram::userFullName(User *user)
{
    return TelegramHelper::fullName(user);
}

QString Telegram::userStatusText(User *user)
{
    return TelegramHelper::statusText(user);
}

Message *Telegram::message(TLInt messageid)
{
    return TelegramCache_message(messageid);
}

QString Telegram::messageFrom(TLInt messageid)
{
    Message* message = TelegramCache_message(messageid);

    if(message)
    {
        if(message->isPost()) // Post = messages from channels
        {
            Chat* chat = TelegramCache_chat(TelegramHelper::identifier(message->toId()));

            if(chat)
                return chat->title();
        }
        else
        {
            User* user = TelegramCache_user(message->fromId());

            if(user)
                return TelegramHelper::fullName(user);
        }
    }

    return "???";
}

QString Telegram::messageText(TLInt messageid)
{
    Message* message = TelegramCache_message(messageid);

    if(!message)
        return QString();

    if(message->media())
        return this->messageMediaText(message->media());

    if(message->action())
        return this->messageActionText(message->action());

    return message->message();
}

FileObject *Telegram::fileObject(Dialog *dialog)
{
    return FileCache_fileObject(dialog);
}

void Telegram::signIn(const QString &phonecode)
{
    this->_initializer->signIn(phonecode);
}

void Telegram::signUp(const QString &firstname, const QString &lastname, const QString &phonecode)
{
    this->_initializer->signUp(firstname, lastname, phonecode);
}

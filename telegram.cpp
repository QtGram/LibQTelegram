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
        disconnect(this->_initializer, &TelegramInitializer::floodWait, this, 0);
        disconnect(this->_initializer, &TelegramInitializer::signUpRequested, this, 0);
        disconnect(this->_initializer, &TelegramInitializer::signUpRequested, this, 0);
        disconnect(this->_initializer, &TelegramInitializer::signInRequested, this, 0);
        disconnect(this->_initializer, &TelegramInitializer::loginCompleted, this, 0);
    }

    this->_initializer = initializer;

    connect(this->_initializer, &TelegramInitializer::floodWait, this, &Telegram::floodWait);
    connect(this->_initializer, &TelegramInitializer::signUpRequested, this, &Telegram::signUpRequested);
    connect(this->_initializer, &TelegramInitializer::signInRequested, this, &Telegram::signInRequested);
    connect(this->_initializer, &TelegramInitializer::loginCompleted, this, &Telegram::loginCompleted);

    emit initializerChanged();
}

QString Telegram::messageMediaText(MessageMedia *messagemedia) const
{
    TLConstructor ctorid = messagemedia->constructorId();

    if(ctorid == TLTypes::MessageMediaWebPage)
    {
        if(!messagemedia->caption().isEmpty())
            return messagemedia->caption();

        return messagemedia->webpage()->url();
    }

    if(ctorid == TLTypes::MessageMediaDocument)
    {
        Document* document = messagemedia->document();

        foreach(DocumentAttribute* attribute, document->attributes())
        {
            if(attribute->constructorId() == TLTypes::DocumentAttributeAnimated)
                return tr("GIF");

            if(attribute->constructorId() == TLTypes::DocumentAttributeSticker)
                return tr("Sticker");

            if(attribute->constructorId() == TLTypes::DocumentAttributeAudio)
                return tr("Audio recording");

            if(attribute->constructorId() == TLTypes::DocumentAttributeHasStickers)
                return tr("Sticker set");

            if(attribute->constructorId() == TLTypes::DocumentAttributeFilename)
                return attribute->fileName();
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

QString Telegram::messageActionText(Message* message) const
{
    User *fromuser = NULL, *inviteruser = NULL, *user = NULL;
    QString fromfullname, fullname, inviterfullname;
    MessageAction* messageaction = message->action();
    TLConstructor ctorid = messageaction->constructorId();

    if(message->fromId())
    {
        fromuser = TelegramCache_user(message->fromId());

        if(fromuser)
            fromfullname = TelegramHelper::fullName(fromuser);
    }

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
        Chat* chat = TelegramCache_chat(TelegramHelper::identifier(message->toId()));

        if(messageaction->channelId())
            return tr("%1 created channel «%2»").arg(fromfullname, (chat ? chat->title().toString() : QString()));

        return tr("%1 created group «%2»").arg(fromfullname, (chat ? chat->title().toString() : QString()));
    }

    if(ctorid == TLTypes::MessageActionChatEditTitle)
        return tr("«%1» changed group name to «%2»").arg(fromfullname, messageaction->title().toString());

    if(ctorid == TLTypes::MessageActionChatEditPhoto)
        return tr("«%1» updated group photo").arg(fromfullname);

    if(ctorid == TLTypes::MessageActionChatDeletePhoto)
        return tr("«%1» deleted group photo").arg(fromfullname);

    if(ctorid == TLTypes::MessageActionChatDeletePhoto)
        return tr("«%1» deleted group photo").arg(fromfullname);

    if(ctorid == TLTypes::MessageActionChatAddUser)
    {
        if(inviteruser)
            return tr("«%1» added «%2» ").arg(inviterfullname, fullname);

        return tr("«%1» has joined the group").arg(fullname);
    }

    if(ctorid == TLTypes::MessageActionChatDeleteUser)
    {
        if(messageaction->userId() && (message->fromId() != messageaction->userId()))
            return tr("«%1» removed «%2»").arg(fromfullname, fullname);

        return tr("«%1» has left the group").arg(fullname);
    }

    if(ctorid == TLTypes::MessageActionChatJoinedByLink)
        return tr("«%1» has joined the group via invite link").arg(fromfullname);

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

QString Telegram::dialogTitle(Dialog *dialog) const
{
    if(!dialog)
        return QString();

    TLInt peerid = TelegramHelper::identifier(dialog->peer());

    if(TelegramHelper::isChat(dialog) || TelegramHelper::isChannel(dialog))
    {
        Chat* chat = TelegramCache_chat(peerid);
        return chat->title();
    }

    User* user = TelegramCache_user(peerid);
    return TelegramHelper::fullName(user);
}

QString Telegram::dialogStatusText(Dialog *dialog) const
{
    if(!dialog)
        return QString();

    TLInt id = TelegramHelper::identifier(dialog);

    if(TelegramHelper::isChannel(dialog) || TelegramHelper::isChat(dialog))
    {
        Chat* chat = TelegramCache_chat(id);

        if(!chat)
            return QString();

        return tr("%1 members").arg(chat->participantsCount());
    }

    User* user = TelegramCache_user(id);

    if(user)
        return TelegramHelper::statusText(user);

    return QString();
}

TelegramObject *Telegram::messageFrom(Message *message) const
{
    if(!message)
        return NULL;

    if(message->isPost()) // Post = messages from channels
    {
        Chat* chat = TelegramCache_chat(TelegramHelper::identifier(message->toId()));

        if(chat)
            return chat;
    }
    else
    {
        User* user = TelegramCache_user(message->fromId());

        if(user)
            return user;
    }

    return NULL;
}

QString Telegram::messageText(Message *message) const
{
    if(!message)
        return QString();

    if(message->media())
    {
        MessageMedia* messagemedia = message->media();

        if(!messagemedia->caption().isEmpty())
            return messagemedia->caption();

        if(messagemedia->constructorId() == TLTypes::MessageMediaWebPage)
            return messagemedia->webpage()->url();
    }

    if(message->action())
        return this->messageActionText(message);

    return message->message();
}

QString Telegram::messagePreview(Message *message) const
{
    if(!message)
        return QString();

    if(message->action())
        return this->messageActionText(message);

    if(message->media())
    {
        QString mediatext = this->messageMediaText(message->media());

        if(!mediatext.isEmpty())
            return mediatext;
    }

    return message->message();
}

void Telegram::signIn(const QString &phonecode)
{
    this->_initializer->signIn(phonecode);
}

void Telegram::signUp(const QString &firstname, const QString &lastname, const QString &phonecode)
{
    this->_initializer->signUp(firstname, lastname, phonecode);
}

void Telegram::resendCode()
{
    this->_initializer->resendCode();
}

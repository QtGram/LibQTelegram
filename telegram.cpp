#include "telegram.h"
#include "mtproto/mtprotoupdatehandler.h"
#include "config/cache/telegramcache.h"
#include "config/cache/filecache.h"
#include "types/telegramhelper.h"

Telegram::Telegram(QObject *parent) : QObject(parent), _initializer(NULL)
{
    connect(DCSessionManager_instance, &DCSessionManager::mainSessionConnectedChanged, this, &Telegram::connectedChanged);
    connect(UpdateHandler_instance, &MTProtoUpdateHandler::syncingChanged, this, &Telegram::syncingChanged);
}

TelegramInitializer *Telegram::initializer() const
{
    return this->_initializer;
}

User *Telegram::me() const
{
    return TelegramConfig_me;
}

int Telegram::apiLayer() const
{
    return TELEGRAM_API_LAYER;
}

bool Telegram::connected() const
{
    if(!DC_MainSession)
        return false;

    DC* dc = SessionToDC(DC_MainSession);
    return dc->state() == DC::ConnectedState;
}

bool Telegram::syncing() const
{
    return UpdateHandler_syncing;
}

void Telegram::setInitializer(TelegramInitializer *initializer)
{
    if(this->_initializer == initializer)
        return;

    if(this->_initializer)
    {
        disconnect(this->_initializer, &TelegramInitializer::floodLock, this, 0);
        disconnect(this->_initializer, &TelegramInitializer::phoneCodeError, this, 0);
        disconnect(this->_initializer, &TelegramInitializer::signUpRequested, this, 0);
        disconnect(this->_initializer, &TelegramInitializer::signInRequested, this, 0);
        disconnect(this->_initializer, &TelegramInitializer::loginCompleted, this, 0);
        disconnect(this->_initializer, &TelegramInitializer::invalidPassword, this, 0);
        disconnect(this->_initializer, &TelegramInitializer::sessionPasswordNeeded, this, 0);
    }

    this->_initializer = initializer;

    connect(this->_initializer, &TelegramInitializer::floodLock, this, &Telegram::floodLock);
    connect(this->_initializer, &TelegramInitializer::phoneCodeError, this, &Telegram::phoneCodeError);
    connect(this->_initializer, &TelegramInitializer::signUpRequested, this, &Telegram::signUpRequested);
    connect(this->_initializer, &TelegramInitializer::signInRequested, this, &Telegram::signInRequested);
    connect(this->_initializer, &TelegramInitializer::loginCompleted, this, &Telegram::loginCompleted);
    connect(this->_initializer, &TelegramInitializer::invalidPassword, this, &Telegram::invalidPassword);
    connect(this->_initializer, &TelegramInitializer::sessionPasswordNeeded, this, &Telegram::sessionPasswordNeeded);
    connect(this->_initializer, &TelegramInitializer::loginCompleted, this, &Telegram::loginCompleted);

    emit initializerChanged();
}

bool Telegram::muteDialog(Dialog *dialog, bool mute)
{
    PeerNotifySettings* notifysettings = dialog->notifySettings();

    if(notifysettings->isSilent() == mute)
        return false;

    notifysettings->setIsSilent(mute);
    notifysettings->setMuteUntil(mute ? Future10Years : 0);
    TelegramCache_store(dialog);

    InputNotifyPeer* inputnotifypeer = TelegramHelper::inputNotifyPeer(dialog, TelegramCache_accessHash(dialog));
    InputPeerNotifySettings* inputpeernotifysettings = TelegramHelper::inputPeerNotifySettings(notifysettings);
    TelegramAPI::accountUpdateNotifySettings(DC_MainSession, inputnotifypeer, inputpeernotifysettings);

    inputpeernotifysettings->deleteLater();
    inputnotifypeer->deleteLater();
    return true;
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
        DocumentAttribute* attribute = NULL;
        Document* document = messagemedia->document();

        if(TelegramHelper::documentHas(document, TLTypes::DocumentAttributeAnimated))
            return tr("GIF");

        if((attribute = TelegramHelper::documentHas(document, TLTypes::DocumentAttributeSticker)))
            return tr("%1 Sticker").arg(attribute->alt().toString());

        if(TelegramHelper::documentHas(document, TLTypes::DocumentAttributeAudio))
            return tr("Audio recording");

        if(TelegramHelper::documentHas(document, TLTypes::DocumentAttributeHasStickers))
            return tr("Sticker Set");

        if((attribute = TelegramHelper::documentHas(document, TLTypes::DocumentAttributeFilename)))
            return attribute->fileName();

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
    {
        if(message->isPost())
            return tr("Channel name changed to «%1»").arg(messageaction->title().toString());

        return tr("«%1» changed group name to «%2»").arg(fromfullname, messageaction->title().toString());
    }

    if(ctorid == TLTypes::MessageActionChatEditPhoto)
    {
        if(message->isPost())
            return tr("Channel photo updated");

        return tr("«%1» updated group photo").arg(fromfullname);
    }

    if(ctorid == TLTypes::MessageActionChatDeletePhoto)
    {
        if(message->isPost())
            return tr("Channel photo deleted");

        return tr("«%1» deleted group photo").arg(fromfullname);
    }

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

    if(message->media() && (message->media()->constructorId() != TLTypes::MessageMediaEmpty))
    {
        MessageMedia* messagemedia = message->media();
        return messagemedia->caption();
    }
    else if(message->action() && (message->action()->constructorId() != TLTypes::MessageActionEmpty))
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

void Telegram::signIn(const QString &phonecode) const
{
    this->_initializer->signIn(phonecode);
}

void Telegram::signUp(const QString &firstname, const QString &lastname, const QString &phonecode) const
{
    this->_initializer->signUp(firstname, lastname, phonecode);
}

void Telegram::sendPassword(const QString &password) const
{
    this->_initializer->sendPassword(password);
}

void Telegram::resendCode() const
{
    this->_initializer->resendCode();
}

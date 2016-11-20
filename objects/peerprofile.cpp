#include "peerprofile.h"
#include "../types/telegramhelper.h"
#include "../cache/telegramcache.h"

PeerProfile::PeerProfile(QObject *parent) : QObject(parent), _telegram(NULL), _peer(NULL), _dialog(NULL), _chat(NULL), _user(NULL)
{

}

Telegram *PeerProfile::telegram() const
{
    return this->_telegram;
}

TelegramObject *PeerProfile::peer() const
{
    return this->_peer;
}

bool PeerProfile::isUser() const
{
    return this->_user != NULL;
}

bool PeerProfile::isChat() const
{
    return this->_chat != NULL;
}

bool PeerProfile::isMegaGroup() const
{
    if(!this->_chat)
        return false;

    return this->_chat->isMegagroup();
}

bool PeerProfile::isBroadcast() const
{
    if(!this->_chat)
        return false;

    return this->_chat->isBroadcast();
}

bool PeerProfile::isContact() const
{
    if(!this->_user)
        return false;

    return this->_user->isContact();
}

bool PeerProfile::isMuted() const
{
    if(this->_dialog)
        return TelegramHelper::isMuted(this->_dialog);

    return false;
}

bool PeerProfile::isMe() const
{
    if(!this->_user)
        return false;

    return this->_user->id() == TelegramConfig_me->id();
}

bool PeerProfile::hasUsername() const
{
    if(!this->_user)
        return false;

    return !this->_user->username().isEmpty();
}

bool PeerProfile::hasPhoneNumber() const
{
    if(!this->_user)
        return false;

    return !this->_user->phone().isEmpty();
}

QString PeerProfile::title() const
{
    if(!this->_telegram || !this->_dialog)
        return QString();

    return this->_telegram->dialogTitle(this->_dialog);
}

QString PeerProfile::username() const
{
    if(!this->_user)
        return QString();

    return this->_user->username();
}

QString PeerProfile::statusText() const
{
    if(!this->_telegram || !this->_dialog)
        return QString();

    return this->_telegram->dialogStatusText(this->_dialog);
}

QString PeerProfile::phoneNumber() const
{
    if(!this->_user)
        return QString();

    return this->_user->phone();
}

void PeerProfile::setTelegram(Telegram *telegram)
{
    if(this->_telegram == telegram)
        return;

    this->_telegram = telegram;
    emit telegramChanged();
}

void PeerProfile::setPeer(TelegramObject *peer)
{
    if(this->_peer == peer)
        return;

    this->_peer = peer;
    emit peerChanged();
}

void PeerProfile::setIsMuted(bool ismuted)
{
    if(!this->_telegram || !this->_dialog)
        return;

    if(this->_telegram->muteDialog(this->_dialog, ismuted))
        emit isMutedChanged();
}

void PeerProfile::classBegin()
{
    /* Does nothing */
}

void PeerProfile::componentComplete()
{
    this->findObjects(this->_peer);
}

void PeerProfile::findObjects(TelegramObject *telegramobject)
{
    if(!telegramobject || !this->_telegram || !this->_peer)
        return;

    switch(telegramobject->constructorId())
    {
        case TLTypes::Message:
        case TLTypes::MessageService:
        {
            Message* message = qobject_cast<Message*>(telegramobject);
            TLInt dialogid = TelegramHelper::messageToDialog(message);
            this->findObjects(TelegramCache_dialog(dialogid));
            break;
        }

        case TLTypes::Dialog:
        {
            this->_dialog = qobject_cast<Dialog*>(telegramobject);
            TLInt dialogid = TelegramHelper::identifier(this->_dialog);

            connect(this->_dialog, &Dialog::notifySettingsChanged, this, &PeerProfile::isMutedChanged);

            if(TelegramHelper::isChat(this->_dialog) || TelegramHelper::isChannel(this->_dialog))
            {
                Chat* chat = TelegramCache_chat(dialogid);

                if(!chat)
                    return;

                this->findObjects(chat);
            }
            else
            {
                User* user = TelegramCache_user(dialogid);

                if(!user)
                    return;

                this->findObjects(user);
            }

            break;
        }

        case TLTypes::User:
        {
            this->_user = qobject_cast<User*>(telegramobject);

            if(!this->_dialog && TelegramCache_hasDialog(this->_user->id()))
            {
                this->_dialog = TelegramCache_dialog(this->_user->id());

                if(this->_dialog)
                    connect(this->_dialog, &Dialog::notifySettingsChanged, this, &PeerProfile::isMutedChanged);
            }

            emit isUserChanged();
            emit isContactChanged();
            emit isMutedChanged();
            emit isMeChanged();
            emit hasUsernameChanged();
            emit hasPhoneNumberChanged();
            emit titleChanged();
            emit usernameChanged();
            emit statusTextChanged();
            emit phoneNumberChanged();
            break;
        }

        case TLTypes::Chat:
        case TLTypes::ChatForbidden:
        case TLTypes::Channel:
        case TLTypes::ChannelForbidden:
        {
            this->_chat = qobject_cast<Chat*>(telegramobject);

            emit isChatChanged();
            emit isMegaGroupChanged();
            emit isBroadcastChanged();
            emit isMutedChanged();
            emit titleChanged();
            emit statusTextChanged();
            break;
        }

        default:
            break;
    }
}

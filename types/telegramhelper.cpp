#include "telegramhelper.h"

TelegramHelper::TelegramHelper()
{

}

Message *TelegramHelper::createMessage(Updates *updates, User* me)
{
    Q_ASSERT((updates->constructorId() == TLTypes::UpdateShortMessage) ||
             (updates->constructorId() == TLTypes::UpdateShortChatMessage));

    Message* message = new Message();
    message->setConstructorId(Message::ctorMessage);
    message->setFlags(updates->flags());
    message->setIsOut(updates->isOut());
    message->setIsMentioned(updates->isMentioned());
    message->setIsMediaUnread(updates->isMediaUnread());
    message->setIsSilent(updates->isSilent());
    message->setId(updates->id());
    message->setMessage(updates->message());
    message->setDate(updates->date());
    message->setFwdFrom(updates->fwdFrom());
    message->setViaBotId(updates->viaBotId());
    message->setReplyToMsgId(updates->replyToMsgId());
    message->setEntities(updates->entities());

    if(updates->constructorId() == TLTypes::UpdateShortMessage)
    {
        Peer* topeer = new Peer();
        topeer->setConstructorId(Peer::ctorPeerUser);
        topeer->setUserId(updates->isOut() ? updates->userId() : me->id());

        message->setFromId(updates->isOut() ? me->id() : updates->userId());
        message->setToId(topeer);
    }
    else if(updates->constructorId() == TLTypes::UpdateShortChatMessage)
    {
        Peer* topeer = new Peer();
        topeer->setConstructorId(Peer::ctorPeerChat);
        topeer->setChatId(updates->chatId());

        message->setFromId(updates->fromId());
        message->setToId(topeer);
    }

    return message;
}

QString TelegramHelper::fullName(User *user)
{
    if(!user)
        return QString();

    if(user->lastName().isEmpty())
        return user->firstName();

    return user->firstName() + " " + user->lastName();
}

QString TelegramHelper::messageText(Message *message)
{
    if(message->media())
    {
        MessageMedia* messagemedia = message->media();
        TLConstructor ctorid = messagemedia->constructorId();

        if(ctorid == TLTypes::MessageMediaWebPage)
        {
            if(!messagemedia->caption().isEmpty())
                return  messagemedia->caption();

            return messagemedia->webpage()->url();
        }

        QString result;

        if(ctorid == TLTypes::MessageMediaContact)
            result = QObject::tr("Contact");
        else if(ctorid == TLTypes::MessageMediaDocument)
            result = QObject::tr("Document");
        else if(ctorid == TLTypes::MessageMediaGame)
            result = QObject::tr("Game");
        else if((ctorid == TLTypes::MessageMediaGeo)  || (ctorid == TLTypes::MessageMediaVenue))
            result = QObject::tr("Location");
        else if(ctorid == TLTypes::MessageMediaPhoto)
            result = QObject::tr("Photo");

        if(!messagemedia->caption().isEmpty())
            result += ", " + message->media()->caption();

        return result;
    }

    return message->message();
}

bool TelegramHelper::isChat(Dialog *dialog)
{
    return dialog->peer()->constructorId() == TLTypes::PeerChat;
}

bool TelegramHelper::isChannel(Dialog *dialog)
{
    return dialog->peer()->constructorId() == TLTypes::PeerChannel;
}

TLInt TelegramHelper::identifier(User *user)
{
    return user->id();
}

TLInt TelegramHelper::identifier(Chat *chat)
{
    return chat->id();
}

TLInt TelegramHelper::identifier(Peer *peer)
{
    if(peer->constructorId() == TLTypes::PeerChannel)
        return peer->channelId();

    if(peer->constructorId() == TLTypes::PeerChat)
        return peer->chatId();

    if(peer->constructorId() == TLTypes::PeerUser)
        return peer->userId();

    Q_ASSERT(false);
    return -1;
}

TLInt TelegramHelper::identifier(Dialog *dialog)
{
    return TelegramHelper::identifier(dialog->peer());
}

TLInt TelegramHelper::identifier(Message *message)
{
    return message->id();
}

TLInt TelegramHelper::dialogIdentifier(Message *message)
{
    if(message->isOut())
        return TelegramHelper::identifier(message->toId());

    return message->fromId();
}

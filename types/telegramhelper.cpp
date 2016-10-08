#include "telegramhelper.h"

TelegramHelper::TelegramHelper()
{

}

Message *TelegramHelper::createMessage(Updates *updates, User* me)
{
    Q_ASSERT(updates->constructorId() == TLTypes::UpdateShortMessage);

    TLInt flags = updates->flags();

    Peer* topeer = new Peer();
    topeer->setConstructorId(Peer::ctorPeerUser);
    topeer->setUserId(updates->isOut() ? updates->userId() : me->id());

    Message* message = new Message();
    message->setConstructorId(Message::ctorMessage);
    message->setFlags(flags);
    message->setIsOut(updates->isOut());
    message->setIsMentioned(updates->isMentioned());
    message->setIsMediaUnread(updates->isMediaUnread());
    message->setIsSilent(updates->isSilent());
    message->setId(updates->id());
    message->setFromId(updates->isOut() ? me->id() : updates->userId());
    message->setToId(topeer);
    message->setMessage(updates->message());
    message->setDate(updates->date());
    message->setFwdFrom(updates->fwdFrom());
    message->setViaBotId(updates->viaBotId());
    message->setReplyToMsgId(updates->replyToMsgId());
    message->setEntities(updates->entities());

    return message;
}

QString TelegramHelper::fullName(User *user)
{
    if(user->lastName().isEmpty())
        return user->firstName();

    return user->firstName() + " " + user->lastName();
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

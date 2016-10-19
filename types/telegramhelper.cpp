#include "telegramhelper.h"
#include <QDateTime>

TelegramHelper::TelegramHelper()
{

}

Message *TelegramHelper::createMessage(Updates *updates, User* me)
{
    Q_ASSERT((updates->constructorId() == TLTypes::UpdateShortMessage) ||
             (updates->constructorId() == TLTypes::UpdateShortChatMessage));

    Message* message = new Message();
    message->setConstructorId(Message::CtorMessage);
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
        topeer->setConstructorId(Peer::CtorPeerUser);
        topeer->setUserId(updates->isOut() ? updates->userId() : me->id());

        message->setFromId(updates->isOut() ? me->id() : updates->userId());
        message->setToId(topeer);
    }
    else if(updates->constructorId() == TLTypes::UpdateShortChatMessage)
    {
        Peer* topeer = new Peer();
        topeer->setConstructorId(Peer::CtorPeerChat);
        topeer->setChatId(updates->chatId());

        message->setFromId(updates->fromId());
        message->setToId(topeer);
    }

    return message;
}

Message *TelegramHelper::createMessage(const QString &text, User* me, Peer* peer)
{
    Message* message = new Message();
    message->setConstructorId(Message::CtorMessage);
    message->setMessage(text);
    message->setDate(QDateTime::currentDateTime().toTime_t());
    message->setFromId(me->id());
    message->setIsOut(true);
    message->setToId(peer);

    return message;
}

InputFileLocation *TelegramHelper::inputFileLocation(FileLocation *filelocation)
{
    InputFileLocation* inputfilelocation = new InputFileLocation();

    inputfilelocation->setConstructorId(TLTypes::InputFileLocation);
    inputfilelocation->setVolumeId(filelocation->volumeId());
    inputfilelocation->setLocalId(filelocation->localId());
    inputfilelocation->setSecret(filelocation->secret());

    return inputfilelocation;
}

InputFileLocation *TelegramHelper::inputFileLocation(Document *document)
{
    InputFileLocation* inputfilelocation = new InputFileLocation();

    inputfilelocation->setConstructorId(TLTypes::InputDocumentFileLocation);
    inputfilelocation->setId(document->id());
    inputfilelocation->setAccessHash(document->accessHash());
    inputfilelocation->setVersion(document->version());

    return inputfilelocation;
}

InputPeer *TelegramHelper::inputPeer(Peer *peer, TLLong accesshash, QObject *parent)
{
    InputPeer* inputpeer = new InputPeer(parent);
    inputpeer->setAccessHash(accesshash);

    if(peer->constructorId() == TLTypes::PeerUser)
    {
        inputpeer->setConstructorId(TLTypes::InputPeerUser);
        inputpeer->setUserId(peer->userId());
    }
    else if(peer->constructorId() == TLTypes::PeerChat)
    {
        inputpeer->setConstructorId(TLTypes::InputPeerChat);
        inputpeer->setChatId(peer->chatId());
    }
    else if(peer->constructorId() == TLTypes::PeerChannel)
    {
        inputpeer->setConstructorId(TLTypes::InputPeerChannel);
        inputpeer->setChannelId(peer->channelId());
    }
    else
        Q_ASSERT(false);

    return inputpeer;
}

QString TelegramHelper::dateString(TLInt timestamp)
{
    QDateTime datetime = QDateTime::fromTime_t(timestamp);
    const QDateTime& currentdatetime = QDateTime::currentDateTime();
    qint64 seconds = datetime.secsTo(currentdatetime);
    int days = datetime.daysTo(currentdatetime);

    if(seconds < 24 * 60 * 60)
        return days ? QObject::tr("Yesterday %1").arg(datetime.toString("HH:mm")) : datetime.toString("HH:mm");

    return datetime.toString("MMM dd, HH:mm");
}

QString TelegramHelper::fullName(User *user)
{
    if(!user)
        return QString();

    if(user->lastName().isEmpty())
        return user->firstName();

    return user->firstName() + " " + user->lastName();
}

QString TelegramHelper::statusText(User *user)
{
    if(user->status())
    {
        if(user->status()->constructorId() == TLTypes::UserStatusLastMonth)
            return QObject::tr("Last month");

        if(user->status()->constructorId() == TLTypes::UserStatusLastWeek)
            return QObject::tr("Last week");

        if(user->status()->constructorId() == TLTypes::UserStatusOffline)
            return QObject::tr("Last seen %1").arg(TelegramHelper::dateString(user->status()->wasOnline()));

        if(user->status()->constructorId() == TLTypes::UserStatusOnline)
            return QObject::tr("Online");

        if(user->status()->constructorId() == TLTypes::UserStatusRecently)
            return QObject::tr("Recently");
    }

    return QObject::tr("Long time ago");
}

DocumentAttribute *TelegramHelper::documentHas(Document *document, TLConstructor attributector)
{
    foreach(DocumentAttribute* attribute, document->attributes())
    {
        if(attribute->constructorId() == attributector)
            return attribute;
    }

    return NULL;
}

bool TelegramHelper::isSticker(Document *document)
{
    return TelegramHelper::documentHas(document, TLTypes::DocumentAttributeSticker) != NULL;
}

bool TelegramHelper::isAnimated(Document *document)
{
    return TelegramHelper::documentHas(document, TLTypes::DocumentAttributeAnimated) != NULL;
}

bool TelegramHelper::isChat(Dialog *dialog)
{
    return TelegramHelper::isChat(dialog->peer());
}

bool TelegramHelper::isChannel(Dialog *dialog)
{
    return TelegramHelper::isChannel(dialog->peer());
}

bool TelegramHelper::isChat(Peer *peer)
{
    return peer->constructorId() == TLTypes::PeerChat;
}

bool TelegramHelper::isChannel(Peer *peer)
{
    return peer->constructorId() == TLTypes::PeerChannel;
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

TLInt TelegramHelper::messageToDialog(Message *message)
{
    if(message->isOut() || TelegramHelper::isChat(message->toId()) || TelegramHelper::isChannel(message->toId()))
        return TelegramHelper::identifier(message->toId());

    return message->fromId();
}

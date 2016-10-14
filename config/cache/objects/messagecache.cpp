#include "messagecache.h"
#include <QFile>
#include <QDir>
#include "../../../types/telegramhelper.h"

MessageCache::MessageCache(QObject *parent) : AbstractCache("messages", parent)
{
}

const MessageCache::MessageList &MessageCache::messages(Dialog* dialog)
{
    TLInt dialogid = TelegramHelper::identifier(dialog);

    if(!this->_cacheloaded.contains(dialogid))
        this->loadId(dialogid);

    this->sortMessages(dialogid);
    return this->_dialogmessages[dialogid];
}

Message *MessageCache::previousMessage(Dialog *dialog, Message *message)
{
     // Load and sort messages, if needed
    const MessageList& messages = this->messages(dialog);
    int idx = messages.indexOf(message);

    if(idx == -1)
    {
        qWarning("Cannot find previous message of %x", message->id());
        return NULL;
    }

    if(idx < (messages.count() - 1))
        return messages[idx + 1];

    return NULL;
}

Message *MessageCache::message(TLInt messageid)
{
    if(!this->_messages.contains(messageid))
    {
        qWarning("Cannot recover message %x from cache", messageid);
        return NULL;
    }

    return this->_messages[messageid];
}

void MessageCache::edit(Message *message)
{
    TLInt id = TelegramHelper::identifier(message);

    if(!this->_messages.contains(id))
    {
        qWarning("Edited message %x not available", id);
        return;
    }

    Message* oldmessage = this->_messages[id];
    TLInt dialogid = TelegramHelper::dialogIdentifier(message);
    int idx = this->_dialogmessages[dialogid].indexOf(oldmessage);

    if(idx == -1)
    {
        qWarning("Cannot edit message id %x", id);
        return;
    }

    this->_dialogmessages[dialogid].replace(idx, message);
    message->setParent(this);
    oldmessage->deleteLater();
}

void MessageCache::doLoadId(TLInt id, MTProtoStream *mtstream)
{
    TLVector<Message*> messages;
    mtstream->readTLVector<Message>(messages);

    this->cache(messages);
    this->_cacheloaded << id;
}

void MessageCache::doSaveId(TLInt id, MTProtoStream *mtstream) const
{
    if(!this->_dialogmessages.contains(id))
        return;

    mtstream->writeTLVector<Message>(this->_dialogmessages[id]);
}

void MessageCache::sortMessages(TLInt dialogid)
{
    if(!this->_dialogmessages.contains(dialogid))
        return;

    MessageList& messagelist = this->_dialogmessages[dialogid];

    std::sort(messagelist.begin(), messagelist.end(), [](Message* msg1, Message* msg2) {
        return msg1->date() > msg2->date();
    });
}

void MessageCache::cache(Message *message)
{
    TLInt id = TelegramHelper::identifier(message);

    if(this->_messages.contains(id))
        return;

    TLInt dialogid = TelegramHelper::dialogIdentifier(message);
    message->setParent(this);

    this->_messages[id] = message;

    if(!this->_dialogmessages.contains(dialogid))
        this->_dialogmessages[dialogid] = MessageList();

    this->_dialogmessages[dialogid] << message;
}

bool MessageCache::uncache(TLInt messageid)
{
    if(!this->_messages.contains(messageid))
        return false;

    Message* message = this->_messages.take(messageid);
    this->deleteId(messageid);
    message->deleteLater();
    return true;
}

void MessageCache::cache(const TLVector<Message *> &messages)
{
    foreach(Message* message, messages)
        this->cache(message);
}

void MessageCache::load()
{
    this->loadTop();
}

void MessageCache::save(const QList<Dialog *> &dialogs) const
{
    QList<TLInt> dialogids = this->_dialogmessages.keys();

    foreach(TLInt dialogid, dialogids)
        this->saveId(dialogid);

    this->saveTop(dialogs);
}

void MessageCache::saveTop(const QList<Dialog *> &dialogs) const
{
    TLVector<Message*> topmessages;

    foreach(Dialog* dialog, dialogs)
    {
        if(!this->_messages.contains(dialog->topMessage()))
        {
            qWarning("Cannot save message %x", dialog->topMessage());
            continue;
        }

        topmessages << this->_messages[dialog->topMessage()];
    }

    this->saveFile("topmessages", [topmessages](MTProtoStream* mtstream) {
        mtstream->writeTLVector<Message>(topmessages);
    });
}

void MessageCache::loadTop()
{
    this->loadFile("topmessages", [this](MTProtoStream* mtstream) {
        TLVector<Message*> messages;
        mtstream->readTLVector<Message>(messages, false, this);
        this->cache(messages);
    });
}

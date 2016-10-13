#include "messagecache.h"
#include "../telegramconfig.h"
#include "../../types/telegramhelper.h"
#include "../../mtproto/mtprotostream.h"
#include <QFile>
#include <QDir>

MessageCache::MessageCache(QObject *parent) : QObject(parent)
{
    this->_msgsstoragepath = TelegramConfig_storagePath + QDir::separator() + "dialogs";

    QDir dir;
    dir.mkpath(this->_msgsstoragepath);
}

const MessageCache::MessageList &MessageCache::messages(Dialog* dialog)
{
    TLInt dialogid = TelegramHelper::identifier(dialog);

    if(!this->_cacheloaded.contains(dialogid))
        this->load(dialogid);

    return this->_dialogmessages[dialogid];
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
    this->sortMessages(dialogid);

    message->setParent(this);
    oldmessage->deleteLater();
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
    this->sortMessages(dialogid);
}

void MessageCache::cache(const TLVector<Message *> &messages)
{
    foreach(Message* message, messages)
        this->cache(message);
}

void MessageCache::load(TLInt dialogid)
{
    QDir dir(this->_msgsstoragepath);
    MTProtoStream mtstream;

    mtstream.load(dir.absoluteFilePath(QString::number(dialogid) + ".cache"));

    TLVector<Message*> messages;
    mtstream.readTLVector<Message>(messages);
    this->cache(messages);

    this->_cacheloaded << dialogid;
}

void MessageCache::load()
{
    this->loadTop();
}

void MessageCache::save(const QList<Dialog *> &dialogs) const
{
    QList<TLInt> dialogids = this->_dialogmessages.keys();

    foreach(TLInt dialogid, dialogids)
        this->save(dialogid);

    this->saveTop(dialogs);
}

void MessageCache::save(TLInt dialogid) const
{
    QDir dir(this->_msgsstoragepath);
    MTProtoStream mtstream;

    if(this->_dialogmessages.contains(dialogid))
        mtstream.writeTLVector<Message>(this->_dialogmessages[dialogid]);

    mtstream.save(dir.absoluteFilePath(QString::number(dialogid) + ".cache"));
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

    QDir dir(this->_msgsstoragepath);
    MTProtoStream mtstream;
    mtstream.writeTLVector<Message>(topmessages);
    mtstream.save(dir.absoluteFilePath("topmessages.cache"));
}

void MessageCache::loadTop()
{
    QDir dir(this->_msgsstoragepath);
    MTProtoStream mtstream;
    mtstream.load(dir.absoluteFilePath("topmessages.cache"));

    TLVector<Message*> messages;
    mtstream.readTLVector<Message>(messages, false, this);
    this->cache(messages);
}

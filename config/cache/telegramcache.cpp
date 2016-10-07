#include "telegramcache.h"

#define CACHE_FOLDER "cache"

TelegramCache* TelegramCache::_instance = NULL;

TelegramCache::TelegramCache(QObject* parent): QObject(parent)
{

}

const QHash<TLInt, Dialog *> &TelegramCache::dialogs() const
{
    return this->_dialogs;
}

const QHash<TLInt, User *> &TelegramCache::users() const
{
    return this->_users;
}

const QHash<TLInt, Chat *> &TelegramCache::chats() const
{
    return this->_chats;
}

const QHash<TLInt, Message *> &TelegramCache::messages() const
{
    return this->_messages;
}

Dialog *TelegramCache::dialog(TLInt id) const
{
    if(!this->_dialogs.contains(id))
    {
        qWarning("Cannot recover dialog %x from cache", id);
        return NULL;
    }

    return this->_dialogs[id];
}

User *TelegramCache::user(TLInt id) const
{
    if(!this->_users.contains(id))
    {
        qWarning("Cannot recover user %x from cache", id);
        return NULL;
    }

    return this->_users[id];
}

Chat *TelegramCache::chat(TLInt id) const
{
    if(!this->_chats.contains(id))
    {
        qWarning("Cannot recover chat %x from cache", id);
        return NULL;
    }

    return this->_chats[id];
}

Message *TelegramCache::message(TLInt id) const
{
    if(!this->_messages.contains(id))
    {
        qWarning("Cannot recover message %x from cache", id);
        return NULL;
    }

    return this->_messages[id];
}

void TelegramCache::cache(const TLVector<Dialog *>& dialogs)
{
    this->cache(dialogs, this->_dialogs);
}

void TelegramCache::cache(const TLVector<User *>& users)
{
    this->cache(users, this->_users);
}

void TelegramCache::cache(const TLVector<Chat *>& chats)
{
    this->cache(chats, this->_chats);
}

void TelegramCache::cache(const TLVector<Message *>& messages)
{
    this->cache(messages, this->_messages);
}

TelegramCache *TelegramCache::cache()
{
    if(!TelegramCache::_instance)
        TelegramCache::_instance = new TelegramCache();

    return TelegramCache::_instance;
}

void TelegramCache::save() const
{
    this->saveToFile<Dialog>(this->_dialogs, "dialogs");
    this->saveToFile<User>(this->_users, "users");
    this->saveToFile<Chat>(this->_chats, "chats");
}

void TelegramCache::load()
{
    this->loadFromFile<Dialog>(this->_dialogs, "dialogs");
    this->loadFromFile<User>(this->_users, "users");
    this->loadFromFile<Chat>(this->_chats, "chats");
}

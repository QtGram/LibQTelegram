#include "telegramcache.h"
#include "../../mtproto/mtprotoupdatehandler.h"

TelegramCache* TelegramCache::_instance = NULL;

TelegramCache::TelegramCache(QObject* parent): QObject(parent)
{
    connect(UpdateHandler_instance, SIGNAL(newMessage(Message*)), this, SLOT(onNewMessage(Message*)));
    connect(UpdateHandler_instance, SIGNAL(editMessage(Message*)), this, SLOT(onEditMessage(Message*)));
    connect(UpdateHandler_instance, SIGNAL(newChat(Chat*)), this, SLOT(cache(Chat*)));
    connect(UpdateHandler_instance, SIGNAL(newUser(User*)), this, SLOT(cache(User*)));
    connect(UpdateHandler_instance, SIGNAL(newUserStatus(Update*)), this, SLOT(onNewUserStatus(Update*)));
    connect(UpdateHandler_instance, SIGNAL(newDraftMessage(Update*)), this, SLOT(onNewDraftMessage(Update*)));
    connect(UpdateHandler_instance, SIGNAL(readHistory(Update*)), this, SLOT(onReadHistory(Update*)));
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

void TelegramCache::onNewMessage(Message *message)
{
    TelegramCache_store(message);

    TLInt dialogid = TelegramHelper::dialogIdentifier(message);

    if(this->_dialogs.contains(dialogid))
    {
        this->_dialogs[dialogid]->setTopMessage(message->id());
        emit dialogsChanged();
    }
}

void TelegramCache::onEditMessage(Message *message)
{
    TLInt id = TelegramHelper::identifier(message);

    if(!this->_messages.contains(id))
    {
        qWarning("Edited message %x not available", id);
        return;
    }

    Message* oldmessage = this->_messages[id];
    this->_messages[id] = message;
    oldmessage->deleteLater();

    emit dialogsChanged();
}

void TelegramCache::onNewUserStatus(Update *update)
{
    Q_ASSERT(update->constructorId() == TLTypes::UpdateUserStatus);

    User* user = TelegramCache_user(update->userId());

    if(!user)
    {
        qWarning("User %x doesn't exist", update->userId());
        return;
    }

    UserStatus* oldstatus = user->status();
    user->setStatus(update->status());
    oldstatus->deleteLater();
}

void TelegramCache::onNewDraftMessage(Update *update)
{
    Q_ASSERT(update->constructorId() == TLTypes::UpdateDraftMessage);

    TLInt id = TelegramHelper::identifier(update->peer());

    if(!this->_dialogs.contains(id))
    {
        qWarning("Cannot update draft of dialog %x", id);
        return;
    }

    Dialog* dialog = this->_dialogs[id];
    DraftMessage* olddraftmessage = dialog->draft();
    dialog->setDraft(update->draft());
    olddraftmessage->deleteLater();

    emit dialogsChanged();
}

void TelegramCache::onReadHistory(Update *update)
{
    Q_ASSERT((update->constructorId() == TLTypes::UpdateReadHistoryInbox) ||
             (update->constructorId() == TLTypes::UpdateReadHistoryOutbox) ||
             (update->constructorId() == TLTypes::UpdateReadChannelInbox) ||
             (update->constructorId() == TLTypes::UpdateReadChannelOutbox));

    bool isout = (update->constructorId() == TLTypes::UpdateReadHistoryOutbox) ||
                 (update->constructorId() == TLTypes::UpdateReadChannelOutbox);

    TLInt id = 0;

    if((update->constructorId() == TLTypes::UpdateReadChannelInbox) || (update->constructorId() == TLTypes::UpdateReadChannelOutbox))
        id = update->channelId();
    else if(update->constructorId() == TLTypes::UpdateReadHistoryInbox)
        id = TelegramHelper::identifier(update->peerUpdatereadhistoryinbox());
    else
        id = TelegramHelper::identifier(update->peer());

    if(!this->_dialogs.contains(id))
    {
        qWarning("Cannot mark dialog %x as read", id);
        return;
    }

    Dialog* dialog = this->_dialogs[id];

    if(isout)
        dialog->setReadOutboxMaxId(update->maxId());
    else
        dialog->setReadInboxMaxId(update->maxId());

    emit dialogsChanged();
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
    this->saveToFile<Message>(this->_messages, "messages");
}

void TelegramCache::load()
{
    this->loadFromFile<Message>(this->_messages, "messages");
    this->loadFromFile<Chat>(this->_chats, "chats");
    this->loadFromFile<User>(this->_users, "users");
    this->loadFromFile<Dialog>(this->_dialogs, "dialogs");
}

void TelegramCache::cache(Dialog *dialog)
{
    this->cache(dialog, this->_dialogs);
}

void TelegramCache::cache(User *user)
{
    this->cache(user, this->_users);
}

void TelegramCache::cache(Chat *chat)
{
    this->cache(chat, this->_chats);
}

void TelegramCache::cache(Message *message)
{
    this->cache(message, this->_messages);
}

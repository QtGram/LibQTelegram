#include "telegramcache.h"
#include "../../mtproto/mtprotoupdatehandler.h"

TelegramCache* TelegramCache::_instance = NULL;

TelegramCache::TelegramCache(QObject* parent): QObject(parent)
{
    this->_messagecache = new MessageCache(this);
    this->_usercache = new UserCache(this);

    connect(UpdateHandler_instance, SIGNAL(editMessage(Message*)), this->_messagecache, SLOT(edit(Message*)));

    connect(UpdateHandler_instance, SIGNAL(newUserStatus(Update*)), this, SLOT(onNewUserStatus(Update*)));
    connect(UpdateHandler_instance, SIGNAL(newUser(User*)), this, SLOT(cache(User*)));

    connect(UpdateHandler_instance, SIGNAL(newMessages(TLVector<Message*>)), this, SLOT(onNewMessages(TLVector<Message*>)));
    connect(UpdateHandler_instance, SIGNAL(newMessage(Message*)), this, SLOT(onNewMessage(Message*)));
    connect(UpdateHandler_instance, SIGNAL(newChat(Chat*)), this, SLOT(cache(Chat*)));
    connect(UpdateHandler_instance, SIGNAL(newDraftMessage(Update*)), this, SLOT(onNewDraftMessage(Update*)));
    connect(UpdateHandler_instance, SIGNAL(readHistory(Update*)), this, SLOT(onReadHistory(Update*)));
}

const QHash<TLInt, Dialog *> &TelegramCache::dialogs() const
{
    return this->_dialogs;
}

const QHash<TLInt, User *> &TelegramCache::users() const
{
    return this->_usercache->users();
}

const QHash<TLInt, Chat *> &TelegramCache::chats() const
{
    return this->_chats;
}

const MessageCache::MessageList &TelegramCache::messages(Dialog *dialog) const
{
    return this->_messagecache->messages(dialog);
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
    return this->_usercache->user(id);
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

Message *TelegramCache::message(TLInt messageid) const
{
    return this->_messagecache->message(messageid);
}

void TelegramCache::cache(const TLVector<Dialog *>& dialogs)
{
    this->cache(dialogs, this->_dialogs);
}

void TelegramCache::cache(const TLVector<User *>& users)
{
    this->_usercache->cache(users);
}

void TelegramCache::cache(const TLVector<Chat *>& chats)
{
    this->cache(chats, this->_chats);
}

void TelegramCache::cache(const TLVector<Message *>& messages)
{
    this->_messagecache->cache(messages);
}

void TelegramCache::onNewMessages(const TLVector<Message *> &messages)
{
    TelegramCache_store(messages);

    foreach(Message* message, messages)
    {
        TLInt dialogid = TelegramHelper::dialogIdentifier(message);

        if(this->_dialogs.contains(dialogid))
            this->_dialogs[dialogid]->setTopMessage(message->id());
        else
            qWarning("Cannot find dialog %x", dialogid);
    }

    emit dialogsChanged();
}

void TelegramCache::onNewMessage(Message *message)
{
    TLVector<Message*> messages;
    messages << message;

    this->onNewMessages(messages);
    emit newMessage(message);
}

void TelegramCache::onNewUserStatus(Update *update)
{
    Q_ASSERT(update->constructorId() == TLTypes::UpdateUserStatus);

    User* user = TelegramCache_user(update->userId());

    if(!user)
        return;

    user->setStatus(update->status());
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
    dialog->setDraft(update->draft());

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
    this->saveToFile<Chat>(this->_chats, "chats");
    this->_usercache->save();
    this->_messagecache->save(this->_dialogs.values());
}

void TelegramCache::load()
{
    this->_messagecache->load();
    this->loadFromFile<Chat>(this->_chats, "chats");
    this->loadFromFile<Dialog>(this->_dialogs, "dialogs");
}

void TelegramCache::cache(Dialog *dialog)
{
    this->cache(dialog, this->_dialogs);
}

void TelegramCache::cache(User *user)
{
    this->_usercache->cache(user);
}

void TelegramCache::cache(Chat *chat)
{
    this->cache(chat, this->_chats);
}

void TelegramCache::cache(Message *message)
{
    this->_messagecache->cache(message);
}

#include "telegramcache.h"
#include "../../mtproto/mtprotoupdatehandler.h"

TelegramCache* TelegramCache::_instance = NULL;

TelegramCache::TelegramCache(QObject* parent): QObject(parent)
{
    this->_database = new CacheDatabase(TelegramConfig_storagePath, this);
    this->_fetcher = new CacheFetcher(this);

    connect(this->_fetcher, SIGNAL(dialogsReceived(TLVector<Dialog*>)), this, SLOT(cacheNotify(TLVector<Dialog*>)));
    connect(this->_fetcher, SIGNAL(usersReceived(TLVector<User*>)), this, SLOT(cache(TLVector<User*>)));
    connect(this->_fetcher, SIGNAL(chatsReceived(TLVector<Chat*>)), this, SLOT(cache(TLVector<Chat*>)));
    connect(this->_fetcher, SIGNAL(messagesReceived(TLVector<Message*>)), this, SLOT(cache(TLVector<Message*>)));
    connect(this->_fetcher, SIGNAL(typingUserReady(Update*)), this, SLOT(onTyping(Update*)));
    connect(this->_fetcher, SIGNAL(userStatusReady(Update*)), this, SLOT(onNewUserStatus(Update*)));

    connect(UpdateHandler_instance, SIGNAL(newUserStatus(Update*)), this, SLOT(onNewUserStatus(Update*)));
    connect(UpdateHandler_instance, SIGNAL(newUser(User*)), this, SLOT(cache(User*)));
    connect(UpdateHandler_instance, SIGNAL(newMessage(Message*)), this, SLOT(cacheNotify(Message*)));
    connect(UpdateHandler_instance, SIGNAL(newChat(Chat*)), this, SLOT(cache(Chat*)));

    connect(UpdateHandler_instance, SIGNAL(newMessages(TLVector<Message*>)), this, SLOT(onNewMessages(TLVector<Message*>)));
    connect(UpdateHandler_instance, SIGNAL(newUsers(TLVector<User*>)), this, SLOT(cache(TLVector<User*>)));
    connect(UpdateHandler_instance, SIGNAL(newChats(TLVector<Chat*>)), this, SLOT(cache(TLVector<Chat*>)));

    connect(UpdateHandler_instance, SIGNAL(newDraftMessage(Update*)), this, SLOT(onNewDraftMessage(Update*)));
    connect(UpdateHandler_instance, SIGNAL(editMessage(Message*)), this, SLOT(editMessage(Message*)));
    connect(UpdateHandler_instance, SIGNAL(deleteMessages(TLVector<TLInt>)), this, SLOT(onDeleteMessages(TLVector<TLInt>)));
    connect(UpdateHandler_instance, SIGNAL(readHistory(Update*)), this, SLOT(onReadHistory(Update*)));
    connect(UpdateHandler_instance, SIGNAL(typing(Update*)), this, SLOT(onTyping(Update*)));
}

QList<Message *> TelegramCache::dialogMessages(Dialog *dialog, int offset, int limit)
{
    return this->_database->messages()->messagesForDialog(dialog, this->_messages, offset, limit, this);
}

User *TelegramCache::user(TLInt id, bool ignoreerror)
{
    if(!this->_users.contains(id))
    {
        User* user = this->_database->users()->get<User>(id, "user", ignoreerror, this);

        if(user)
            this->_users[id] = user;

        return user;
    }

    return this->_users[id];
}

Chat *TelegramCache::chat(TLInt id, bool ignoreerror)
{
    if(!this->_chats.contains(id))
    {
        Chat* chat = this->_database->chats()->get<Chat>(id, "chat", ignoreerror, this);

        if(chat)
            this->_chats[chat->id()] = chat;

        return chat;
    }

    return this->_chats[id];
}

Message *TelegramCache::message(TLInt id, bool ignoreerror)
{
    if(!this->_messages.contains(id))
    {
        Message* message = this->_database->messages()->get<Message>(id, "message", ignoreerror, this);

        if(message)
            this->_messages[message->id()] = message;

        return message;
    }

    return this->_messages[id];
}

Dialog *TelegramCache::dialog(TLInt id, bool ignoreerror) const
{
    foreach(Dialog* dialog, this->_dialogs)
    {
        if(TelegramHelper::identifier(dialog) == id)
            return dialog;
    }

    if(!ignoreerror)
        qWarning("Cannot get dialog with id %d", id);

    return NULL;
}

bool TelegramCache::hasDialog(TLInt id) const
{
    Dialog* dialog = this->dialog(id, true);

    if(!dialog) // Try from Database
        return this->_database->dialogs()->contains(id);

    return true;
}

void TelegramCache::cache(const TLVector<Dialog *>& dialogs)
{
    this->_database->transaction([this, dialogs](QSqlQuery& queryobj) {
        this->_database->dialogs()->prepareInsert(queryobj);

        foreach(Dialog* dialog, dialogs) {
            dialog->setParent(this);
            this->_dialogs << dialog;
            this->_database->dialogs()->insertQuery(queryobj, dialog);
        }
    });
}

void TelegramCache::cache(const TLVector<User *>& users)
{
    this->_database->transaction([this, users](QSqlQuery& queryobj) {
        this->_database->users()->prepareInsert(queryobj);

        foreach(User* user, users) {
            if(this->_users.contains(user->id()))
                continue;

            user->setParent(this);
            this->_users[user->id()] = user;
            this->_database->users()->insertQuery(queryobj, user);
        }
    });
}

void TelegramCache::cache(const TLVector<Chat *>& chats)
{
    this->_database->transaction([this, chats](QSqlQuery& queryobj) {
        this->_database->chats()->prepareInsert(queryobj);

        foreach(Chat* chat, chats) {
            if(this->_chats.contains(chat->id()))
                continue;

            chat->setParent(this);
            this->_chats[chat->id()] = chat;
            this->_database->chats()->insertQuery(queryobj, chat);
        }
    });
}

void TelegramCache::cache(const TLVector<Message *>& messages)
{
    this->_database->transaction([this, messages](QSqlQuery& queryobj) {
        this->_database->messages()->prepareInsert(queryobj);

        foreach(Message* message, messages) {
            if(this->_messages.contains(message->id()))
                continue;

            message->setParent(this);
            this->_messages[message->id()] = message;
            this->_database->messages()->insertQuery(queryobj, message);
        }
    });
}

void TelegramCache::onNewMessages(const TLVector<Message *> &messages)
{
    bool dialogsupdated = false;
    this->cache(messages);

    foreach(Message* message, messages)
    {
        TLInt dialogid = TelegramHelper::messageToDialog(message);
        Dialog* dialog = this->dialog(dialogid, true);

        if(dialog)
        {
            dialogsupdated = true;
            dialog->setTopMessage(message->id());
            this->cache(dialog);
            continue;
        }
        else
            this->_fetcher->getDialog(TelegramHelper::inputPeer(message));
    }

    if(dialogsupdated)
        emit dialogsChanged();
}

void TelegramCache::cacheNotify(Message *message)
{
    TLVector<Message*> messages;
    messages << message;

    this->onNewMessages(messages);
    emit newMessage(message);
}

void TelegramCache::cacheNotify(const TLVector<Dialog *> &dialogs)
{
    this->cache(dialogs);
    emit newDialogs(dialogs);
}

void TelegramCache::onNewUserStatus(Update *update)
{
    Q_ASSERT(update->constructorId() == TLTypes::UpdateUserStatus);
    User* user = this->user(update->userId(), true);

    if(!user)
    {
        this->_fetcher->getUser(update);
        return;
    }

    user->setStatus(update->status());
    this->cache(user);
}

void TelegramCache::onNewDraftMessage(Update *update)
{
    Q_ASSERT(update->constructorId() == TLTypes::UpdateDraftMessage);
    Dialog* dialog = this->dialog(TelegramHelper::identifier(update->peer()));

    if(!dialog)
        return;

    dialog->setDraft(update->draft());
    this->cache(dialog);
    emit dialogsChanged();
}

void TelegramCache::editMessage(Message *message)
{
    Message* oldmessage = this->message(message->id());

    if(!oldmessage)
    {
        qWarning("Edited message %x not available", message->id());
        return;
    }

    this->cache(message);
    Dialog* dialog = this->dialog(TelegramHelper::messageToDialog(message));

    if(dialog && (dialog->topMessage() == message->id()))
        emit dialogsChanged();

    oldmessage->deleteLater();
}

void TelegramCache::onDeleteMessages(const TLVector<TLInt> &messageids)
{
    bool updatedialogs = false;

    foreach(TLInt messageid, messageids)
    {
        Message* message = this->message(messageid);

        if(!message)
            continue;

        Dialog* dialog = this->dialog(TelegramHelper::messageToDialog(message));

        if(dialog && (dialog->topMessage() == message->id()))
        {
            Message* prevmessage = this->_database->messages()->previousMessage(message, this->_messages, this);

            if(prevmessage)
            {
                dialog->setTopMessage(prevmessage->id());
                this->_database->dialogs()->insert(dialog);
                updatedialogs = true;
            }
        }

        emit deleteMessage(message);
        this->_database->messages()->remove(messageid);
    }

    if(updatedialogs)
        emit dialogsChanged();
}

void TelegramCache::onReadHistory(Update *update)
{
    Q_ASSERT((update->constructorId() == TLTypes::UpdateReadHistoryInbox)  ||
             (update->constructorId() == TLTypes::UpdateReadHistoryOutbox) ||
             (update->constructorId() == TLTypes::UpdateReadChannelInbox)  ||
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

    Dialog* dialog = this->dialog(id);

    if(!dialog)
        return;

    if(isout)
        dialog->setReadOutboxMaxId(update->maxId());
    else
        dialog->setReadInboxMaxId(update->maxId());

    this->cache(dialog);
    emit dialogsChanged();
}

void TelegramCache::onTyping(Update *update)
{
    Q_ASSERT((update->constructorId() == TLTypes::UpdateUserTyping) ||
             (update->constructorId() == TLTypes::UpdateChatUserTyping));

    User* user = this->user(update->userId(), true);

    if(!user)
    {
        this->_fetcher->getUser(update);
        return;
    }

    TLInt dialogid = (update->constructorId() == TLTypes::UpdateChatUserTyping) ? update->chatId() : update->userId();
    Dialog* dialog = this->dialog(dialogid);
    emit typing(dialog, update->action());
}

TelegramCache *TelegramCache::cache()
{
    if(!TelegramCache::_instance)
        TelegramCache::_instance = new TelegramCache();

    return TelegramCache::_instance;
}

void TelegramCache::load()
{
    this->_database->dialogs()->populate(this->_dialogs, this);
    this->_database->users()->populateContacts(this->_contacts, this);
}

const QList<Dialog *>& TelegramCache::dialogs() const
{
    return this->_dialogs;
}

const QList<User *> &TelegramCache::contacts() const
{
    return this->_contacts;
}

void TelegramCache::cache(Dialog *dialog)
{
    dialog->setParent(this);

    this->_dialogs << dialog;
    this->_database->dialogs()->insert(dialog);
}

void TelegramCache::cache(User *user)
{
    if(!this->_users.contains(user->id()))
        return;

    user->setParent(this);

    this->_users[user->id()] = user;
    this->_database->users()->insert(user);
}

void TelegramCache::cache(Chat *chat)
{
    if(!this->_chats.contains(chat->id()))
        return;

    chat->setParent(this);

    this->_chats[chat->id()] = chat;
    this->_database->chats()->insert(chat);
}

void TelegramCache::cache(Message *message)
{
    if(this->_messages.contains(message->id()))
        return;

    message->setParent(this);

    this->_messages[message->id()] = message;
    this->_database->messages()->insert(message);
}

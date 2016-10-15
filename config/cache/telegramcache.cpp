#include "telegramcache.h"
#include "../../mtproto/mtprotoupdatehandler.h"

TelegramCache* TelegramCache::_instance = NULL;

TelegramCache::TelegramCache(QObject* parent): QObject(parent)
{
    this->_database = new CacheDatabase(TelegramConfig_storagePath, this);

    connect(UpdateHandler_instance, SIGNAL(newUserStatus(Update*)), this, SLOT(onNewUserStatus(Update*)));
    connect(UpdateHandler_instance, SIGNAL(newUser(User*)), this, SLOT(cache(User*)));
    connect(UpdateHandler_instance, SIGNAL(newMessages(TLVector<Message*>)), this, SLOT(onNewMessages(TLVector<Message*>)));
    connect(UpdateHandler_instance, SIGNAL(newMessage(Message*)), this, SLOT(onNewMessage(Message*)));
    connect(UpdateHandler_instance, SIGNAL(newChat(Chat*)), this, SLOT(cache(Chat*)));
    connect(UpdateHandler_instance, SIGNAL(editMessage(Message*)), this, SLOT(onEditMessage(Message*)));
    connect(UpdateHandler_instance, SIGNAL(deleteMessages(TLVector<TLInt>)), this, SLOT(onDeleteMessages(TLVector<TLInt>)));
    connect(UpdateHandler_instance, SIGNAL(newDraftMessage(Update*)), this, SLOT(onNewDraftMessage(Update*)));
    connect(UpdateHandler_instance, SIGNAL(readHistory(Update*)), this, SLOT(onReadHistory(Update*)));
}

QList<Message *> TelegramCache::dialogMessages(Dialog *dialog, int limit)
{
    return this->_database->messages()->messagesForDialog(dialog, this->_messages, limit, this);
}

User *TelegramCache::user(TLInt id)
{
    if(!this->_users.contains(id))
    {
        User* user = this->_database->users()->get<User>(id, this);

        if(user)
            this->_users[id] = user;

        return user;
    }

    return this->_users[id];
}

Chat *TelegramCache::chat(TLInt id)
{
    if(!this->_chats.contains(id))
    {
        Chat* chat = this->_database->chats()->get<Chat>(id, this);

        if(chat)
            this->_chats[chat->id()] = chat;

        return chat;
    }

    return this->_chats[id];
}

Message *TelegramCache::message(TLInt id)
{
    if(!this->_messages.contains(id))
    {
        Message* message = this->_database->messages()->get<Message>(id, this);

        if(message)
            this->_messages[message->id()] = message;

        return message;
    }

    return this->_messages[id];
}

Dialog *TelegramCache::dialog(TLInt id)
{
    foreach(Dialog* dialog, this->_dialogs)
    {
        if(TelegramHelper::identifier(dialog) == id)
            return dialog;
    }

    qWarning("Cannot find dialog %d", id);
    return NULL;
}

void TelegramCache::cache(const TLVector<Dialog *>& dialogs)
{
    this->_database->transaction([this, dialogs](QSqlQuery& queryobj) {
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
    this->cache(messages);

    foreach(Message* message, messages)
    {
        TLInt dialogid = TelegramHelper::messageToDialog(message);
        Dialog* dialog = this->dialog(dialogid);

        if(dialog)
        {
            dialog->setTopMessage(message->id());
            this->cache(dialog);
        }
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
    User* user = this->user(update->userId());

    if(!user)
        return;

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

void TelegramCache::onEditMessage(Message *message)
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

#include "telegramcache.h"
#include "../../mtproto/mtprotoupdatehandler.h"

TelegramCache* TelegramCache::_instance = NULL;

TelegramCache::TelegramCache(QObject* parent): QObject(parent)
{
    this->_database = new CacheDatabase(TelegramConfig_storagePath, this);
    this->_fetcher = new CacheFetcher(this);

    connect(this->_fetcher, SIGNAL(dialogsReceived(TLVector<Dialog*>)), this, SLOT(onDialogsReceived(TLVector<Dialog*>)));
    connect(this->_fetcher, SIGNAL(usersReceived(TLVector<User*>)), this, SLOT(cache(TLVector<User*>)));
    connect(this->_fetcher, SIGNAL(chatsReceived(TLVector<Chat*>)), this, SLOT(cache(TLVector<Chat*>)));
    connect(this->_fetcher, SIGNAL(messagesReceived(TLVector<Message*>)), this, SLOT(cache(TLVector<Message*>)));

    connect(UpdateHandler_instance, SIGNAL(newUserStatus(Update*)), this, SLOT(onNewUserStatus(Update*)));
    connect(UpdateHandler_instance, SIGNAL(newUser(User*)), this, SLOT(cache(User*)));
    connect(UpdateHandler_instance, SIGNAL(newMessage(Message*)), this, SLOT(insert(Message*)));
    connect(UpdateHandler_instance, SIGNAL(newChat(Chat*)), this, SLOT(cache(Chat*)));

    connect(UpdateHandler_instance, SIGNAL(newMessages(TLVector<Message*>)), this, SLOT(onNewMessages(TLVector<Message*>)));
    connect(UpdateHandler_instance, SIGNAL(newUsers(TLVector<User*>)), this, SLOT(cache(TLVector<User*>)));
    connect(UpdateHandler_instance, SIGNAL(newChats(TLVector<Chat*>)), this, SLOT(cache(TLVector<Chat*>)));

    connect(UpdateHandler_instance, SIGNAL(newDraftMessage(Update*)), this, SLOT(onNewDraftMessage(Update*)));
    connect(UpdateHandler_instance, SIGNAL(editMessage(Message*)), this, SLOT(onEditMessage(Message*)));
    connect(UpdateHandler_instance, SIGNAL(deleteMessages(TLVector<TLInt>)), this, SLOT(onDeleteMessages(TLVector<TLInt>)));
    connect(UpdateHandler_instance, SIGNAL(deleteChannelMessages(TLInt,TLVector<TLInt>)), this, SLOT(onDeleteChannelMessages(TLInt,TLVector<TLInt>)));
    connect(UpdateHandler_instance, SIGNAL(readHistory(Update*)), this, SLOT(onReadHistory(Update*)));
    connect(UpdateHandler_instance, SIGNAL(typing(Update*)), this, SLOT(onTyping(Update*)));
}

QList<Message *> TelegramCache::dialogMessages(Dialog *dialog, int offset, int limit)
{
    return this->_database->messages()->messagesForDialog(dialog, this->_messages, offset, limit, this);
}

QList<Message *> TelegramCache::lastDialogMessages(Dialog *dialog)
{
    return this->_database->messages()->lastMessagesForDialog(dialog, this->_messages, this);
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

Message *TelegramCache::message(MessageId messageid, Dialog *dialog, bool ignoreerror)
{
    if(!messageid)
        return NULL;

    if(dialog && TelegramHelper::isChannel(dialog))
        messageid = TelegramHelper::identifier(messageid, dialog);

    if(!this->_messages.contains(messageid))
    {
        Message* message = this->_database->messages()->get<Message>(messageid, "message", ignoreerror, this);

        if(message)
            this->_messages[messageid] = message;

        return message;
    }

    return this->_messages[messageid];
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

void TelegramCache::markAsRead(Dialog *dialog, TLInt inmaxid, TLInt outmaxid)
{
    dialog->setReadInboxMaxId(inmaxid);
    dialog->setReadOutboxMaxId(outmaxid);
    dialog->setUnreadCount(qMin(dialog->topMessage() - inmaxid, 0));

    this->cache(dialog);
    emit readHistory(dialog);
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
            MessageId messageid = TelegramHelper::identifier(message);

            if(this->_messages.contains(messageid))
                continue;

            message->setParent(this);
            this->_messages[messageid] = message;
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
        Dialog* dialog = this->dialog(dialogid, true);

        if(dialog)
        {
            dialog->setTopMessage(message->id());

            if(!message->isOut())
                dialog->setUnreadCount(dialog->unreadCount() + 1);

            this->cache(dialog);

            emit newMessage(message);
            emit dialogNewMessage(dialog);
        }
        else if(!this->hasDialog(dialogid))
            this->_fetcher->getDialog(TelegramHelper::inputPeer(message));
    }
}

void TelegramCache::insert(Message *message)
{
    TLVector<Message*> messages;
    messages << message;

    this->onNewMessages(messages);
}

void TelegramCache::onDialogsReceived(const TLVector<Dialog *> &dialogs)
{
    this->cache(dialogs);
    emit newDialogs(dialogs);
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
    emit dialogNewDraftMessage(dialog);
}

void TelegramCache::onEditMessage(Message *message)
{
    this->cache(message);
    Dialog* dialog = this->dialog(TelegramHelper::messageToDialog(message));

    if(dialog && (dialog->topMessage() == message->id()))
        emit dialogEditMessage(dialog);

    emit editMessage(message);
}

void TelegramCache::onDeleteMessages(const TLVector<TLInt> &messageids)
{
    foreach(TLInt messageid, messageids)
        this->eraseMessage(messageid);
}

void TelegramCache::onDeleteChannelMessages(TLInt channelid, const TLVector<TLInt> &messageids)
{
    foreach(TLInt messageid, messageids)
    {
        MessageId fullmessageid = TelegramHelper::identifier(messageid, channelid);
        this->eraseMessage(fullmessageid);
    }
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

    if(!isout)
    {
        dialog->setReadInboxMaxId(update->maxId());
        dialog->setUnreadCount(dialog->topMessage() - update->maxId());
    }
    else
    {
        dialog->setReadOutboxMaxId(update->maxId());

        if(update->maxId() > dialog->readInboxMaxId())
            dialog->setUnreadCount(0);
    }

    this->cache(dialog);

    emit readHistory(dialog);
    emit dialogUnreadCountChanged(dialog);
}

void TelegramCache::onTyping(Update *update)
{
    Q_ASSERT((update->constructorId() == TLTypes::UpdateUserTyping) ||
             (update->constructorId() == TLTypes::UpdateChatUserTyping));

    User* user = this->user(update->userId());

    if(!user)
        return;

    TLInt dialogid = (update->constructorId() == TLTypes::UpdateChatUserTyping) ? update->chatId() : update->userId();
    Dialog* dialog = this->dialog(dialogid);
    emit typing(dialog, update->action());
}

void TelegramCache::eraseMessage(MessageId messageid)
{
    Message* message = this->message(messageid, NULL);

    if(!message)
        return;

    this->_database->messages()->remove(messageid);
    Dialog* dialog = this->dialog(TelegramHelper::messageToDialog(message));

    if(dialog && (dialog->topMessage() == message->id()))
    {
        Message* topmessage = this->_database->messages()->topMessage(dialog, this->_messages, this);

        if(!topmessage) // Dialog is empty
        {
            dialog->setReadInboxMaxId(0);
            dialog->setReadOutboxMaxId(0);
            dialog->setTopMessage(0);
            dialog->setUnreadCount(0);
        }
        else
        {
            dialog->setTopMessage(topmessage->id());

            if(topmessage->isOut())
                dialog->setReadOutboxMaxId(topmessage->id());
            else
                dialog->setReadInboxMaxId(topmessage->id());
        }

        this->_database->dialogs()->insert(dialog); // Update dialog
        emit dialogDeleteMessage(dialog);
    }

    emit deleteMessage(message);
    this->_messages.remove(messageid);
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
    if(this->_dialogs.indexOf(dialog) == -1) // NOTE: Linear search?
    {
        dialog->setParent(this);
        this->_dialogs << dialog;
    }

    this->_database->dialogs()->insert(dialog);
}

void TelegramCache::cache(User *user)
{
    user->setParent(this);

    this->_users[user->id()] = user;
    this->_database->users()->insert(user);
}

void TelegramCache::cache(Chat *chat)
{
    chat->setParent(this);

    this->_chats[chat->id()] = chat;
    this->_database->chats()->insert(chat);
}

void TelegramCache::cache(Message *message)
{
    Message* oldmessage = NULL;
    MessageId messageid = TelegramHelper::identifier(message);

    if(this->_messages.contains(messageid))
        oldmessage = this->_messages[messageid];

    message->setParent(this);

    this->_messages[messageid] = message;
    this->_database->messages()->insert(message);

    if(oldmessage)
        oldmessage->deleteLater();
}

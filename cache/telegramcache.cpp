#include "telegramcache.h"
#include "../mtproto/mtprotoupdatehandler.h"

TelegramCache* TelegramCache::_instance = NULL;

TelegramCache::TelegramCache(QObject* parent): QObject(parent), _unreadcount(0)
{
    this->_database = new CacheDatabase(TelegramConfig_storagePath, this);
    this->_fetcher = new CacheFetcher(this);

    connect(this->_fetcher, SIGNAL(usersReceived(TLVector<User*>)), this, SLOT(cache(TLVector<User*>)));
    connect(this->_fetcher, SIGNAL(chatFullReceived(ChatFull*)), this, SLOT(cache(ChatFull*)));

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
    connect(UpdateHandler_instance, SIGNAL(notifySettings(NotifyPeer*,PeerNotifySettings*)), this, SLOT(onNotifySettings(NotifyPeer*,PeerNotifySettings*)));
    connect(UpdateHandler_instance, SIGNAL(readHistory(Update*)), this, SLOT(onReadHistory(Update*)));
    connect(UpdateHandler_instance, SIGNAL(webPage(WebPage*)), this, SLOT(onWebPage(WebPage*)));
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

ChatFull *TelegramCache::chatFull(TLInt id)
{
    if(!this->_chatfull.contains(id))
    {
        ChatFull* chatfull = this->_database->chatFull()->get<ChatFull>(id, "chatfull", true, this);

        if(!chatfull)
        {
            Chat* chat = this->chat(id);

            if(!chat)
                return NULL;

            this->_fetcher->getFullChat(chat);
        }
        else
            this->_chatfull[chatfull->id()] = chatfull;

        return chatfull;
    }

    return this->_chatfull[id];
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

TLLong TelegramCache::accessHash(Dialog *dialog)
{
    if(TelegramHelper::isChannel(dialog) || TelegramHelper::isChat(dialog))
    {
        Chat* chat = this->chat(TelegramHelper::identifier(dialog));

        if(chat)
            return chat->accessHash();
    }
    else
    {
        User* user = this->user(TelegramHelper::identifier(dialog));

        if(user)
            return user->accessHash();
    }

    return 0;
}

void TelegramCache::setUnreadCount(int unreadcount)
{
    if(this->_unreadcount == unreadcount)
        return;

    this->_unreadcount = unreadcount;
    emit unreadCountChanged();
}

void TelegramCache::markAsRead(Dialog *dialog, TLInt inmaxid, TLInt outmaxid)
{
    dialog->setReadInboxMaxId(inmaxid);
    dialog->setReadOutboxMaxId(outmaxid);

    this->updateUnreadCount(dialog, this->checkUnreadMessages(dialog));
    this->cache(dialog);

    emit dialogUnreadCountChanged(dialog);
}

void TelegramCache::clearHistory(Dialog *dialog)
{
    dialog->setTopMessage(0);

    this->updateUnreadCount(dialog, 0);
    this->cache(dialog);

    TLVector<MessageId> deletedmessages;
    this->_database->messages()->removeDialogMessages(TelegramHelper::identifier(dialog), deletedmessages);

    foreach(MessageId messageid, deletedmessages)
    {
        if(!this->_messages.contains(messageid))
            continue;

        Message* message = this->_messages.take(messageid);
        emit deleteMessage(message);
        message->deleteLater();
    }

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
        bool newcontacts = false;
        this->_database->users()->prepareInsert(queryobj);

        foreach(User* user, users) {
            if(this->_users.contains(user->id()))
                continue;

            user->setParent(this);
            this->_users[user->id()] = user;

            if(user->isContact()) {
                newcontacts = true;
                this->_contacts << user;
            }

            this->_database->users()->insertQuery(queryobj, user);
        }

        if(newcontacts)
            emit contactsUpdated();
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

TLVector<Message*> TelegramCache::cache(const TLVector<Message *>& messages)
{
    TLVector<Message*> newmessages;

    this->_database->transaction([this, messages, &newmessages](QSqlQuery& queryobj) {
        this->_database->messages()->prepareInsert(queryobj);

        foreach(Message* message, messages) {
            MessageId messageid = TelegramHelper::identifier(message);

            if(this->_messages.contains(messageid))
                continue;

            newmessages << message;
            message->setParent(this);

            this->_messages[messageid] = message;
            this->_database->messages()->insertQuery(queryobj, message);
        }
    });

    return newmessages;
}

void TelegramCache::onNewMessages(const TLVector<Message *> &messages)
{
    TLVector<Message*> newmessages = this->cache(messages);

    foreach(Message* message, newmessages)
    {
        if(TelegramHelper::messageIsWebPagePending(message))
            this->_database->pendingWebPages()->insert(message);
        else if(TelegramHelper::messageIsAction(message))
            this->checkMessageAction(message);

        TLInt dialogid = TelegramHelper::messageToDialog(message);
        Dialog* dialog = this->dialog(dialogid, true);

        if(!dialog)
        {
            dialog = TelegramHelper::createDialog(message);
            this->insert(dialog);
        }

        dialog->setTopMessage(message->id());

        if(!message->isOut())
            this->updateUnreadCount(dialog, dialog->unreadCount() + 1);

        this->cache(dialog);
        emit newMessage(message);
        emit dialogNewMessage(dialog);
    }
}

void TelegramCache::insert(Message *message)
{
    TLVector<Message*> messages;
    messages << message;

    this->onNewMessages(messages);
}

void TelegramCache::insert(Dialog *dialog)
{
    this->_dialogs << dialog;

    emit newDialogs((TLVector<Dialog*>() << dialog));
}

void TelegramCache::remove(Dialog *dialog)
{
    int idx = this->_dialogs.indexOf(dialog);

    if(idx != -1)
        this->_dialogs.removeAt(idx);

    TLVector<MessageId> deletedmessages;
    TLInt dialogid = TelegramHelper::identifier(dialog);

    this->_database->dialogs()->remove(dialogid);
    this->_database->messages()->removeDialogMessages(dialogid, deletedmessages);

    foreach(MessageId messageid, deletedmessages)
    {
        if(!this->_messages.contains(messageid))
            continue;

        Message* message = this->_messages.take(messageid);
        emit deleteMessage(message);
        message->deleteLater();
    }
}

void TelegramCache::remove(Message *message)
{
    this->eraseMessage(TelegramHelper::identifier(message));
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
        dialog->setReadInboxMaxId(update->maxId());
    else
        dialog->setReadOutboxMaxId(update->maxId());

    this->updateUnreadCount(dialog, this->checkUnreadMessages(dialog));
    this->cache(dialog);

    emit readHistory(dialog);
    emit dialogUnreadCountChanged(dialog);
}

void TelegramCache::onWebPage(WebPage *webpage)
{
    MessageId messageid = this->_database->pendingWebPages()->messageId(webpage);

    if(!messageid)
        return;

    this->_database->pendingWebPages()->remove(webpage->id());
    Message* message = this->message(messageid, NULL);

    if(!message || !message->media())
        return;

    message->media()->setWebpage(webpage);
    this->cache(message);
    emit messageUpdated(message);
}

void TelegramCache::onNotifySettings(NotifyPeer *notifypeer, PeerNotifySettings *notifysettings)
{
    if(notifypeer->constructorId() != TLTypes::NotifyPeer)
        return;

    TLInt dialogid = TelegramHelper::identifier(notifypeer->peer());;
    Dialog* dialog = this->dialog(dialogid);

    if(!dialog)
        return;

    dialog->setNotifySettings(notifysettings);
    this->cache(dialog);
    emit dialogNotifySettingsChanged(dialog);
}

void TelegramCache::eraseMessage(MessageId messageid)
{
    Message* message = this->message(messageid, NULL, true);

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

            this->updateUnreadCount(dialog, 0);
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

void TelegramCache::checkMessageAction(Message *message)
{
    MessageAction* messageaction = message->action();
    TLInt dialogid = TelegramHelper::messageToDialog(message);

    if(messageaction->constructorId() == TLTypes::MessageActionChatEditTitle)
    {
        Chat* chat = this->chat(dialogid);

        if(!chat)
            return;

        chat->setTitle(messageaction->title());
        this->cache(chat);

        Dialog* dialog = this->dialog(dialogid);

        if(!dialog)
            return;

        emit titleChanged(dialog);
    }
    else if(messageaction->constructorId() == TLTypes::MessageActionChatEditPhoto)
    {
        Chat* chat = this->chat(dialogid);

        if(!chat)
            return;

        chat->setPhoto(TelegramHelper::chatPhoto(messageaction->photo()));
        this->cache(chat);

        Dialog* dialog = this->dialog(dialogid);

        if(!dialog)
            return;

        emit photoChanged(dialog);
    }
}

void TelegramCache::updateUnreadCount(Dialog *dialog, TLInt newunreadcount)
{
    this->_unreadcount -= dialog->unreadCount();
    this->_unreadcount += newunreadcount;

    dialog->setUnreadCount(newunreadcount);
    emit dialogUnreadCountChanged(dialog);
    emit unreadCountChanged();
}

int TelegramCache::checkUnreadMessages(Dialog *dialog)
{
    if(!dialog->topMessage() || (dialog->readOutboxMaxId() >= dialog->topMessage()))
        return 0;

    Message* message = this->message(dialog->topMessage(), dialog);

    if(!message || message->isOut() || (dialog->topMessage() == dialog->readInboxMaxId()))
        return 0;

    return this->_database->messages()->messagesCount(dialog, dialog->readInboxMaxId(), dialog->topMessage());
}

TelegramCache *TelegramCache::cache()
{
    if(!TelegramCache::_instance)
        TelegramCache::_instance = new TelegramCache();

    return TelegramCache::_instance;
}

void TelegramCache::load()
{
    this->_unreadcount = this->_database->dialogs()->populate(this->_dialogs, this);
    this->_database->users()->populateContacts(this->_contacts, this);

    emit unreadCountChanged();
}

int TelegramCache::unreadCount()
{
    return this->_unreadcount;
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

    if(oldmessage == message) // We are only updating this message
    {
        this->_database->messages()->insert(message);
        return;
    }

    message->setParent(this);

    this->_messages[messageid] = message;
    this->_database->messages()->insert(message);

    if(oldmessage)
        oldmessage->deleteLater();
}

void TelegramCache::cache(ChatFull *chatfull)
{
    if(this->_chatfull.contains(chatfull->id()))
        return;

    chatfull->setParent(this);
    this->_chatfull[chatfull->id()] = chatfull;
    this->_database->chatFull()->insert(chatfull);

    Dialog* dialog = this->dialog(chatfull->id());

    if(!dialog)
        return;

    emit chatFullChanged(dialog);
}

#ifndef TELEGRAMCACHE_H
#define TELEGRAMCACHE_H

#define TelegramCache_instance TelegramCache::cache()
#define TelegramCache_load TelegramCache::cache()->load();
#define TelegramCache_store(objs) TelegramCache::cache()->cache(objs);

#define TelegramCache_hasDialog(dialogid) TelegramCache::cache()->hasDialog(dialogid)
#define TelegramCache_markAsRead(dialog, inmaxid, outmaxid) TelegramCache::cache()->markAsRead(dialog, inmaxid, outmaxid)
#define TelegramCache_clearHistory(dialog) TelegramCache::cache()->clearHistory(dialog)
#define TelegramCache_accessHash(dialog) TelegramCache::cache()->accessHash(dialog)

#define TelegramCache_unreadCount TelegramCache::cache()->unreadCount()
#define TelegramCache_dialogs TelegramCache::cache()->dialogs()
#define TelegramCache_contacts TelegramCache::cache()->contacts()

#define TelegramCache_dialog(dialogid) TelegramCache::cache()->dialog(dialogid)
#define TelegramCache_user(userid) TelegramCache::cache()->user(userid)
#define TelegramCache_chat(chatid) TelegramCache::cache()->chat(chatid)
#define TelegramCache_chatFull(chatid) TelegramCache::cache()->chatFull(chatid)
#define TelegramCache_message(messageid, dialog) TelegramCache::cache()->message(messageid, dialog)

#define TelegramCache_insert(obj) TelegramCache::cache()->insert(obj)
#define TelegramCache_remove(obj) TelegramCache::cache()->remove(obj)

#define TelegramCache_messages(dialog, offset, limit) TelegramCache::cache()->dialogMessages(dialog, offset, limit)
#define TelegramCache_lastDialogMessages(dialog) TelegramCache::cache()->lastDialogMessages(dialog)

#include <QObject>
#include <QList>
#include <QDir>
#include "../autogenerated/types/types.h"
#include "../mtproto/mtprotoreply.h"
#include "../types/telegramhelper.h"
#include "../config/telegramconfig.h"
#include "database/cachedatabase.h"
#include "cachefetcher.h"

class TelegramCache: public QObject
{
    Q_OBJECT

    private:
        TelegramCache(QObject *parent = NULL);

    public:
        static TelegramCache* cache();
        void load();
        int unreadCount();
        const QList<Dialog*> &dialogs() const;
        const QList<User*> &contacts() const;
        QList<Message*> dialogMessages(Dialog* dialog, int offset, int limit);
        QList<Message*> lastDialogMessages(Dialog* dialog);
        User* user(TLInt id, bool ignoreerror = false);
        Chat* chat(TLInt id, bool ignoreerror = false);
        ChatFull *chatFull(TLInt id);
        Message* message(MessageId messageid, Dialog* dialog, bool ignoreerror = false);
        Dialog* dialog(TLInt id, bool ignoreerror = false) const;
        bool hasDialog(TLInt id) const;
        TLLong accessHash(Dialog* dialog);

    public slots:
        void markAsRead(Dialog* dialog, TLInt inmaxid, TLInt outmaxid);
        void clearHistory(Dialog* dialog);
        void cache(Dialog* dialog);
        void cache(User* user);
        void cache(Chat* chat);
        void cache(Message* message);
        void cache(ChatFull* chatfull);
        void cache(const TLVector<Dialog *> &dialogs);
        void cache(const TLVector<Message *> &messages);
        void cache(const TLVector<User *> &users);
        void cache(const TLVector<Chat *> &chats);
        void insert(Message* message);
        void insert(Dialog* dialog);
        void remove(Dialog* dialog);

    private slots:
        void onNewUserStatus(Update* update);
        void onNewDraftMessage(Update* update);
        void onEditMessage(Message* message);
        void onNewMessages(const TLVector<Message*>& messages);
        void onDeleteMessages(const TLVector<TLInt>& messageids);
        void onDeleteChannelMessages(TLInt channelid, const TLVector<TLInt>& messageids);
        void onNotifySettings(NotifyPeer* notifypeer, PeerNotifySettings* notifysettings);
        void onReadHistory(Update* update);
        void onWebPage(WebPage* webpage);

    private:
        void eraseMessage(MessageId messageid);
        void checkMessageAction(Message* message);
        void updateUnreadCount(Dialog* dialog, TLInt unreadcount);
        int checkUnreadMessages(Dialog* dialog);

    signals:
        void dialogUnreadCountChanged(Dialog* dialog);
        void dialogNotifySettingsChanged(Dialog* dialog);
        void dialogNewMessage(Dialog* dialog);
        void dialogNewDraftMessage(Dialog* dialog);
        void dialogDeleteMessage(Dialog* dialog);
        void dialogEditMessage(Dialog* dialog);
        void titleChanged(Dialog* dialog);
        void photoChanged(Dialog* dialog);
        void newDialogs(const TLVector<Dialog *> &dialogs);
        void newMessage(Message* message);
        void editMessage(Message* message);
        void deleteMessage(Message* message);
        void messageUpdated(Message* message);
        void readHistory(Dialog* dialog);
        void chatFullChanged(Dialog* dialog);
        void participantsCountChanged();
        void contactsUpdated();
        void unreadCountChanged();

    private:
        QList<Dialog*> _dialogs;
        QList<User*> _contacts;
        QHash<TLInt, Chat*> _chats;
        QHash<TLInt, User*> _users;
        QHash<TLInt, ChatFull*> _chatfull;
        QHash<MessageId, Message*> _messages;
        CacheDatabase* _database;
        CacheFetcher* _fetcher;
        int _unreadcount;

    private:
        static TelegramCache* _instance;
};

#endif // TELEGRAMCACHE_H

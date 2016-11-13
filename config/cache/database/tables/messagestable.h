#ifndef MESSAGESTABLE_H
#define MESSAGESTABLE_H

#include "databasetable.h"

class MessagesTable : public DatabaseTable
{
    Q_OBJECT

    public:
        explicit MessagesTable(QObject *parent = 0);
        virtual void createSchema();
        virtual void insertQuery(QSqlQuery& queryobj, TelegramObject *telegramobject);
        void removeDialogMessages(TLInt dialogid, TLVector<MessageId> &deletedmessages);
        Message* topMessage(Dialog* dialog, QHash<MessageId, Message*>& messages, QObject* parent);
        QList<Message*> messagesForDialog(Dialog* dialog, QHash<MessageId, Message *> &messages, int offset, int limit, QObject* parent) const;
        QList<Message*> lastMessagesForDialog(Dialog* dialog, QHash<MessageId, Message *> &messages, QObject* parent) const;
        TLInt messagesCount(Dialog *dialog, TLInt minid, TLInt maxid) const;

    private:
        bool prepareDelete(TLInt dialogid, TLVector<MessageId> &messages);
        void loadMessages(QSqlQuery& queryobj, QList<Message *> &result, QHash<MessageId, Message*>& messages, bool append, QObject* parent) const;
        Message* loadMessage(QSqlQuery& queryobj, QHash<MessageId, Message*>& messages, QObject* parent) const;
};

#endif // MESSAGESTABLE_H

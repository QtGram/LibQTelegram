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
        Message* previousMessage(Message* message, QHash<MessageId, Message*>& messages, QObject* parent) const;
        QList<Message*> messagesForDialog(Dialog* dialog, QHash<MessageId, Message *> &messages, int offset, int limit, QObject* parent) const;
};

#endif // MESSAGESTABLE_H

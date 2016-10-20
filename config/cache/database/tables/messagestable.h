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
        Message* previousMessage(Message*message, QHash<TLInt, Message*>& messages, QObject* parent);
        QList<Message*> messagesForDialog(Dialog* dialog, QHash<TLInt, Message*>& messages, int offset, int limit, QObject* parent);
};

#endif // MESSAGESTABLE_H

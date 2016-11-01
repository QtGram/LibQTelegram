#ifndef MESSAGESMODEL_H
#define MESSAGESMODEL_H

#include "abstract/telegrammodel.h"

class MessagesModel : public TelegramModel
{
    Q_OBJECT

    Q_PROPERTY(Dialog* dialog READ dialog WRITE setDialog NOTIFY dialogChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(bool isChat READ isChat NOTIFY isChatChanged)
    Q_PROPERTY(bool isBroadcast READ isBroadcast NOTIFY isBroadcastChanged)
    Q_PROPERTY(bool isMegaGroup READ isMegaGroup NOTIFY isMegaGroupChanged)
    Q_PROPERTY(bool isWritable READ isWritable NOTIFY isWritableChanged)
    Q_PROPERTY(int loadCount READ loadCount CONSTANT FINAL)

    public:
        enum MessageRoles {
            MessageFrom = Qt::UserRole + 10,
            MessageText,
            MessageDateRole,
            IsMessageNewRole,
            IsMessageOutRole,
            IsMessageServiceRole,
            IsMessageUnreadRole,
            IsMessageEditedRole,
        };

    public:
        explicit MessagesModel(QObject *parent = 0);
        Dialog* dialog() const;
        void setDialog(Dialog* dialog);
        QString title() const;
        QString statusText() const;
        void setLoadCount(int loadcount);
        bool isChat() const;
        bool isBroadcast() const;
        bool isMegaGroup() const;
        bool isWritable() const;
        int loadCount() const;
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual int rowCount(const QModelIndex &) const;
        virtual QHash<int, QByteArray> roleNames() const;

    public slots:
        void loadHistory();
        void loadMore();
        void sendMessage(const QString& text);

    private slots:
        void onMessagesGetHistoryReplied(MTProtoReply* mtreply);
        void onMessagesSendMessageReplied(MTProtoReply* mtreply);
        void onMessagesReadHistoryReplied(MTProtoReply* mtreply);
        void onNewMessage(Message* message);
        void onEditMessage(Message* message);
        void onDeleteMessage(Message *message);

    private:
        void setFirstNewMessage();
        TLInt inboxMaxId() const;
        TLInt outboxMaxId() const;
        void markAsRead();
        int indexOf(Message* message) const;
        bool ownMessage(Message* message) const;
        QString messageFrom(Message *message) const;
        void createInputPeer();
        virtual void telegramReady();

    signals:
        void dialogChanged();
        void titleChanged();
        void statusTextChanged();
        void isChatChanged();
        void isBroadcastChanged();
        void isMegaGroupChanged();
        void isWritableChanged();

    private:
        TLVector<Message*> _messages;
        QList<Message*> _pendingmessages;
        InputPeer* _inputpeer;
        Dialog* _dialog;
        TLInt _firstnewmsgid;
        bool _atend;
        bool _atstart;
        int _loadcount;
};

#endif // MESSAGESMODEL_H

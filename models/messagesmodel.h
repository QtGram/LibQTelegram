#ifndef MESSAGESMODEL_H
#define MESSAGESMODEL_H

#include "abstract/telegrammodel.h"

class MessagesModel : public TelegramModel
{
    Q_OBJECT

    Q_PROPERTY(Dialog* dialog READ dialog WRITE setDialog NOTIFY dialogChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(int newMessageIndex READ newMessageIndex NOTIFY newMessageIndexChanged)
    Q_PROPERTY(bool isChat READ isChat NOTIFY isChatChanged)
    Q_PROPERTY(bool isBroadcast READ isBroadcast NOTIFY isBroadcastChanged)
    Q_PROPERTY(bool isMegaGroup READ isMegaGroup NOTIFY isMegaGroupChanged)
    Q_PROPERTY(bool isWritable READ isWritable NOTIFY isWritableChanged)

    public:
        enum MessageRoles {
            MessageFromRole = Qt::UserRole + 10,
            MessageTextRole,
            MessageDateRole,
            MessageHasReplyRole,
            ReplyItemRole,
            ReplyFromRole,
            ReplyTextRole,
            IsMessageForwardedRole,
            IsMessageNewRole,
            IsMessageOutRole,
            IsMessageServiceRole,
            IsMessageUnreadRole,
            IsMessageEditedRole,
            NeedsPeerImageRole,
        };

    public:
        explicit MessagesModel(QObject *parent = 0);
        Dialog* dialog() const;
        void setDialog(Dialog* dialog);
        QString title() const;
        QString statusText() const;
        int newMessageIndex() const;
        bool isChat() const;
        bool isBroadcast() const;
        bool isMegaGroup() const;
        bool isWritable() const;

    public: // Overrides
        virtual bool canFetchMore(const QModelIndex &) const;
        virtual void fetchMore(const QModelIndex &);
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual int rowCount(const QModelIndex &) const;
        virtual QHash<int, QByteArray> roleNames() const;

    public slots:
        void sendMessage(const QString& text);
        void replyMessage(const QString& text, Message* replymessage);
        void editMessage(const QString& text, Message *editmessage);

    private slots:
        void onMessagesGetHistoryReplied(MTProtoReply* mtreply);
        void onMessagesSendMessageReplied(MTProtoReply* mtreply);
        void onMessagesReadHistoryReplied(MTProtoReply* mtreply);
        void onSendStatusUpdated(Dialog* dialog);
        void onNewMessage(Message* message);
        void onEditMessage(Message* message);
        void onDeleteMessage(Message *message);

    private:
        int loadHistoryFromCache();
        int indexOf(Message* message) const;
        TLInt inboxMaxId() const;
        TLInt outboxMaxId() const;
        QString messageFrom(Message *message) const;
        void sendMessage(const QString& text, TLInt replymsgid);
        void setFirstNewMessage();
        void markAsRead();
        bool ownMessage(Message* message) const;
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
        void newMessageIndexChanged();

    private:
        TLVector<Message*> _messages;
        QList<Message*> _pendingmessages;
        InputPeer* _inputpeer;
        Dialog* _dialog;
        TLInt _newmessageindex;
        bool _fetchmore;
        bool _atstart;
        int _loadcount;
};

#endif // MESSAGESMODEL_H

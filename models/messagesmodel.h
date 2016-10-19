#ifndef MESSAGESMODEL_H
#define MESSAGESMODEL_H

#include "abstract/telegrammodel.h"

class MessagesModel : public TelegramModel
{
    Q_OBJECT

    Q_PROPERTY(Dialog* dialog READ dialog WRITE setDialog NOTIFY dialogChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(int loadCount READ loadCount WRITE setLoadCount NOTIFY loadCountChanged)
    Q_PROPERTY(bool isChat READ isChat NOTIFY isChatChanged)
    Q_PROPERTY(bool isBroadcast READ isBroadcast NOTIFY isBroadcastChanged)
    Q_PROPERTY(bool isMegaGroup READ isMegaGroup NOTIFY isMegaGroupChanged)

    public:
        enum MessageRoles {
            MessageFrom = Qt::UserRole + 10,
            MessageText,
            IsServiceMessageRole,
        };

    public:
        explicit MessagesModel(QObject *parent = 0);
        Dialog* dialog() const;
        void setDialog(Dialog* dialog);
        QString title() const;
        QString statusText() const;
        int loadCount() const;
        void setLoadCount(int loadcount);
        bool isChat() const;
        bool isBroadcast() const;
        bool isMegaGroup() const;
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual int rowCount(const QModelIndex &) const;
        virtual QHash<int, QByteArray> roleNames() const;

    public slots:
        void loadMore();
        void sendMessage(const QString& text);

    private slots:
        void onMessagesGetHistoryReplied(MTProtoReply* mtreply);
        void onMessagesSendMessageReplied(MTProtoReply* mtreply);
        void onNewMessage(Message* message);
        void onDeleteMessage(Message *message);

    private:
        QString messageFrom(Message *message) const;
        TLInt maxId() const;
        void createInputPeer();
        virtual void telegramReady();

    signals:
        void dialogChanged();
        void titleChanged();
        void statusTextChanged();
        void loadCountChanged();
        void isChatChanged();
        void isBroadcastChanged();
        void isMegaGroupChanged();

    private:
        TLVector<Message*> _messages;
        QList<Message*> _pendingmessages;
        InputPeer* _inputpeer;
        Dialog* _dialog;
        bool _athistoryend;
        int _loadcount;
};

#endif // MESSAGESMODEL_H

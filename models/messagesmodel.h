#ifndef MESSAGESMODEL_H
#define MESSAGESMODEL_H

#include "abstract/telegrammodel.h"

class MessagesModel : public TelegramModel
{
    Q_OBJECT

    Q_PROPERTY(Dialog* dialog READ dialog WRITE setDialog NOTIFY dialogChanged)
    Q_PROPERTY(int loadCount READ loadCount WRITE setLoadCount NOTIFY loadCountChanged)

    public:
        explicit MessagesModel(QObject *parent = 0);
        Dialog* dialog() const;
        void setDialog(Dialog* dialog);
        int loadCount() const;
        void setLoadCount(int loadcount);
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
        TLInt maxId() const;
        void createInputPeer();
        virtual void telegramReady();

    signals:
        void dialogChanged();
        void loadCountChanged();

    private:
        TLVector<Message*> _messages;
        QList<Message*> _pendingmessages;
        InputPeer* _inputpeer;
        Dialog* _dialog;
        bool _athistoryend;
        int _loadcount;
};

#endif // MESSAGESMODEL_H

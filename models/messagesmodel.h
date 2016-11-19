#ifndef MESSAGESMODEL_H
#define MESSAGESMODEL_H

#include "abstract/telegrammodel.h"
#include "../config/cache/filecache.h"

class MessagesModel : public TelegramModel
{
    Q_OBJECT

    Q_ENUMS(MessageActions)

    Q_PROPERTY(Dialog* dialog READ dialog WRITE setDialog NOTIFY dialogChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY statusTextChanged)
    Q_PROPERTY(int newMessageIndex READ newMessageIndex NOTIFY newMessageIndexChanged)
    Q_PROPERTY(bool isActive READ isActive WRITE setIsActive NOTIFY isActiveChanged)
    Q_PROPERTY(bool isMuted READ isMuted WRITE setIsMuted NOTIFY isMutedChanged)
    Q_PROPERTY(bool isChat READ isChat NOTIFY isChatChanged)
    Q_PROPERTY(bool isBroadcast READ isBroadcast NOTIFY isBroadcastChanged)
    Q_PROPERTY(bool isMegaGroup READ isMegaGroup NOTIFY isMegaGroupChanged)
    Q_PROPERTY(bool isWritable READ isWritable NOTIFY isWritableChanged)

    public:
        enum MessageActions {
            TypingAction,
            CancelAction,
            RecordVideoAction,
            UploadVideoAction,
            RecordAudioAction,
            UploadAudioAction,
            UploadPhotoAction,
            UploadDocumentAction,
            GeoLocationAction,
            ChooseContactAction,
            GamePlayAction,
            GameStopAction,
        };

        enum MessageRoles {
            MessageFromRole = Qt::UserRole + 10,
            MessageTextRole,
            MessageCaptionRole,
            MessageDateRole,
            MessageHasReplyRole,
            ReplyItemRole,
            ReplyFromRole,
            ReplyTextRole,
            IsMessageForwardedRole,
            IsMessageNewRole,
            IsMessageOutRole,
            IsMessageServiceRole,
            IsMessageMediaRole,
            IsMessageUnreadRole,
            IsMessageEditedRole,
            IsMessagePendingRole,
            NeedsPeerImageRole,
            ForwardedFromPeerRole,
            ForwardedFromNameRole,
        };

    public:
        explicit MessagesModel(QObject *parent = 0);
        Dialog* dialog() const;
        QString title() const;
        QString statusText() const;
        int newMessageIndex() const;
        bool isChat() const;
        bool isBroadcast() const;
        bool isMegaGroup() const;
        bool isWritable() const;
        bool isActive() const;
        bool isMuted() const;
        void setDialog(Dialog* dialog);
        void setIsActive(bool isactive);
        void setIsMuted(bool ismuted);
        Q_INVOKABLE Message* get(int index) const;

    public: // Overrides
        virtual bool canFetchMore(const QModelIndex&) const;
        virtual void fetchMore(const QModelIndex & = QModelIndex());
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual int rowCount(const QModelIndex &) const;
        virtual QHash<int, QByteArray> roleNames() const;

    public slots:
        void sendMessage(const QString& text);
        void replyMessage(const QString& text, Message* replymessage);
        void forwardMessage(Dialog *fromdialog, Message* forwardmessage);
        void editMessage(const QString& text, Message *editmessage);
        void sendFile(const QUrl& filepath, const QString &caption);
        void sendPhoto(const QUrl& filepath, const QString& caption);
        void sendLocation(TLDouble latitude, TLDouble longitude);
        void sendAction(int action);

    private slots:
        void onMessagesGetHistoryReplied(MTProtoReply* mtreply);
        void onMessagesSendMessageReplied(MTProtoReply* mtreply);
        void onMessagesSendMediaReplied(MTProtoReply* mtreply);
        void onReadHistoryReplied(MTProtoReply* mtreply);
        void onReadHistory(Dialog* dialog);
        void onTitleChanged(Dialog* dialog);
        void onNewMessage(Message* message);
        void onEditMessage(Message* message);
        void onDeleteMessage(Message *message);
        void onUpdateMessage(Message *message);
        void onNotifySettingsChanged(Dialog* dialog);
        void onUploadCompleted();
        void updateStatusText(Dialog* dialog);
        void resetAction();
        void markAsRead();

    private:
        TLConstructor getAction(int action) const;
        int insertionPoint(Message* message) const;
        int loadHistoryFromCache();
        int indexOf(Message* message) const;
        int indexOf(TLInt messageid) const;
        TLInt inboxMaxId() const;
        TLInt outboxMaxId() const;
        QString messageFrom(Message *message) const;
        void sendMedia(const QUrl& filepath, const QString& caption, FileUploader::MediaType mediatype);
        void sendMessage(const QString& text, TLInt replymsgid);
        void updateMessage(TLInt localmessageid, Message* message);
        void insertMessage(TLInt localmessageid, Message* message);
        void setFirstNewMessage();
        void createInput();
        void terminateInitialization();
        bool ownMessage(Message* message) const;

    protected:
        virtual void telegramReady();

    signals:
        void dialogChanged();
        void titleChanged();
        void statusTextChanged();
        void isChatChanged();
        void isBroadcastChanged();
        void isMegaGroupChanged();
        void isWritableChanged();
        void isActiveChanged();
        void isMutedChanged();
        void newMessageIndexChanged();

    private:
        TLVector<Message*> _messages;
        QHash<TLInt, Message*> _pendingmessages;
        InputPeer* _inputpeer;
        InputChannel* _inputchannel;
        Dialog* _dialog;
        QTimer* _timaction;
        TLInt _newmessageindex;
        TLInt _newmessageid;
        TLInt _lastreadedinbox;
        bool _isactive;
        bool _fetchmore;
        int _loadcount;
};

#endif // MESSAGESMODEL_H

#ifndef DIALOGSMODEL_H
#define DIALOGSMODEL_H

#include "abstract/telegrammodel.h"

class DialogsModel : public TelegramModel
{
    Q_OBJECT

    Q_PROPERTY(Dialog* cloudDialog READ cloudDialog CONSTANT FINAL)

    public:
        enum DialogRoles {
            TitleRole = Qt::UserRole + 10,
            DraftMessageRole,
            UnreadCountRole,
            PeerActionRole,
            TopMessageRole,
            TopMessageFromRole,
            TopMessageTextRole,
            TopMessageDateRole,
            IsTopMessageUnreadRole,
            IsTopMessageOutRole,
            IsTopMessageServiceRole,
            IsMegaGroupRole,
            IsBroadcastRole,
            IsChatRole,
            IsCloudRole,
            IsMutedRole,
        };

    public:
        explicit DialogsModel(QObject *parent = 0);
        Dialog* cloudDialog();

    public:
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
        virtual int rowCount(const QModelIndex& = QModelIndex()) const;
        virtual QHash<int, QByteArray> roleNames() const;

    public slots:
        int indexOf(Dialog* dialog) const;
        Dialog* getDialog(TLInt dialogid) const;
        void removeDialog(int index);
        void clearHistory(int index);
        QString dialogTitle(Dialog* dialog) const;

    private:
        void doRemoveDialog(int index);
        int insertionPoint(Dialog* insdialog, int fromidx = -1) const;
        int indexOf(TLInt dialogid) const;
        QString messageFrom(Message *message) const;
        QString firstMessageLine(Message* message) const;
        QString draftMessage(Dialog *dialog) const;

    private slots:
        void onSendStatusUpdated(Dialog* dialog);
        void onDialogUnreadCountChanged(Dialog* dialog);
        void onDialogNotifySettingsChanged(Dialog* dialog);
        void onDialogNewMessage(Dialog* dialog);
        void onDialogNewDraftMessage(Dialog* dialog);
        void onDialogDeleteMessage(Dialog* dialog);
        void onDialogDeleted(Dialog* dialog);
        void onDialogEditMessage(Dialog* dialog);
        void onNewDialogs(const TLVector<Dialog*>& newdialogs);
        void onReadHistory(Dialog* dialog);
        void onTitleChanged(Dialog* dialog);

    protected:
        virtual void telegramReady();

    private:
        QList<Dialog*> _dialogs;
        Dialog* _clouddialog;
};

#endif // DIALOGSMODEL_H

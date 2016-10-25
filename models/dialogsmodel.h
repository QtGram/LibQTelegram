#ifndef DIALOGSMODEL_H
#define DIALOGSMODEL_H

#include "abstract/telegrammodel.h"

class DialogsModel : public TelegramModel
{
    Q_OBJECT

    public:
        enum DialogRoles {
            TitleRole = Qt::UserRole + 10,
            DraftMessageRole,
            UnreadCountRole,
            TopMessageRole,
            TopMessageFromRole,
            TopMessageTextRole,
            IsTopMessageOutRole,
            IsTopMessageServiceRole,
            IsMegaGroupRole,
            IsBroadcastRole,
            IsChatRole,
        };

    public:
        explicit DialogsModel(QObject *parent = 0);
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual int rowCount(const QModelIndex &) const;
        virtual QHash<int, QByteArray> roleNames() const;

    private:
        int indexOf(TLInt dialogid) const;
        QString messageFrom(Message *message) const;
        QString firstMessageLine(Message* message) const;
        QString draftMessage(Dialog *dialog) const;

    private slots:
        void sortDialogs();
        void onNewDialogs(const TLVector<Dialog*>& dialogs);
        void onReadInbox(Dialog* dialog);
        void onNewMessage(Message* message);

    protected:
        virtual void telegramReady();

    private:
        QList<Dialog*> _dialogs;
};

#endif // DIALOGSMODEL_H

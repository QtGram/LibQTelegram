#ifndef CONTACTSMODEL_H
#define CONTACTSMODEL_H

#include "abstract/telegrammodel.h"

class ContactsModel : public TelegramModel
{
    Q_OBJECT

    public:
        enum ContactRoles {
            FirstLetterRole = Qt::UserRole + 10,
            FullNameRole,
            StatusText,
        };

    public:
        explicit ContactsModel(QObject *parent = 0);

    public slots:
        void createDialog(User* user);
        void createChat(const QString &title, const QVariantList &users);
        void createChannel(const QString &title, const QString &description);

    protected:
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual int rowCount(const QModelIndex &) const;
        virtual QHash<int, QByteArray> roleNames() const;
        virtual void telegramReady();

    private slots:
        void onNewDialogs(const TLVector<Dialog*>& dialogs);
        void onCreateChannelOrChatReplied(MTProtoReply* mtreply);
        void loadContacts();

    private:
        TLInt getDialogId(Updates* updates) const;

    signals:
        void dialogCreated(Dialog* dialog);

    private:
        QList<User*> _contacts;
        TLInt _pendingdialogid;
};

#endif // CONTACTSMODEL_H

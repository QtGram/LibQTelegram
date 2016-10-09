#ifndef CONTACTSMODEL_H
#define CONTACTSMODEL_H

#include "abstract/telegramsortfilterproxymodel.h"
#include "usersmodel.h"

class ContactsModel : public TelegramSortFilterProxyModel
{
    Q_OBJECT

    public:
        explicit ContactsModel(QObject *parent = 0);

    protected:
        virtual bool filterAcceptsRow(int sourcerow, const QModelIndex &) const;
        virtual void updateTelegram(Telegram* telegram);

    private:
        UsersModel* _usersmodel;
};

#endif // CONTACTSMODEL_H

#include "contactsmodel.h"
#include "usersmodel.h"

ContactsModel::ContactsModel(QObject *parent) : TelegramSortFilterProxyModel(parent)
{
    this->_usersmodel = new UsersModel(this);
    this->setSourceModel(this->_usersmodel);
}

bool ContactsModel::filterAcceptsRow(int sourcerow, const QModelIndex &) const
{
    User* user = this->_usersmodel->data(this->_usersmodel->index(sourcerow, 0), UsersModel::ItemRole).value<User*>();

    if(user)
        return user->isContact();

    return true;
}

void ContactsModel::updateTelegram(Telegram *telegram)
{
    this->_usersmodel->setTelegram(telegram);
}

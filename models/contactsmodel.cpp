#include "contactsmodel.h"
#include "../config/cache/telegramcache.h"

ContactsModel::ContactsModel(QObject *parent) : TelegramModel(parent)
{
}

QVariant ContactsModel::data(const QModelIndex &index, int role) const
{
    if(role == ContactsModel::ItemRole)
        return QVariant::fromValue(this->_contacts[index.row()]);

    if(role == ContactsModel::FirstLetterRole)
    {
        User* user = this->_contacts[index.row()];
        return TelegramHelper::fullName(user).at(0).toUpper();
    }

    if(role == ContactsModel::FullNameRole)
    {
        User* user = this->_contacts[index.row()];
        return TelegramHelper::fullName(user);
    }

    if(role == ContactsModel::StatusText)
    {
        User* user = this->_contacts[index.row()];
        return TelegramHelper::statusText(user);
    }

    return QVariant();
}

int ContactsModel::rowCount(const QModelIndex &) const
{
    return this->_contacts.length();
}

QHash<int, QByteArray> ContactsModel::roleNames() const
{
    QHash<int, QByteArray> roles = this->initRoles();

    roles[ContactsModel::FirstLetterRole] = "firstLetter";
    roles[ContactsModel::FullNameRole] = "fullName";
    roles[ContactsModel::StatusText] = "statusText";

    return roles;
}

void ContactsModel::telegramReady()
{
    this->_contacts = TelegramCache_contacts;
    this->sortUsers();
}

void ContactsModel::sortUsers()
{
    this->beginResetModel();

    std::sort(this->_contacts.begin(), this->_contacts.end(), [](User* usr1, User* usr2) {
        return TelegramHelper::fullName(usr1) < TelegramHelper::fullName(usr2);
    });

    this->endResetModel();
}

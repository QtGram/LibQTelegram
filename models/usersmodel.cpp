#include "usersmodel.h"
#include "../config/cache/telegramcache.h"

UsersModel::UsersModel(QObject *parent) : TelegramModel(parent)
{

}

QVariant UsersModel::data(const QModelIndex &index, int role) const
{
    if(role == UsersModel::ItemRole)
        return QVariant::fromValue(this->_users[index.row()]);

    if(role == UsersModel::FirstLetterRole)
    {
        User* user = this->_users[index.row()];
        return TelegramHelper::fullName(user).at(0).toUpper();
    }

    return QVariant();
}

int UsersModel::rowCount(const QModelIndex &) const
{
    return this->_users.length();
}

QHash<int, QByteArray> UsersModel::roleNames() const
{
    QHash<int, QByteArray> roles = this->initRoles();
    roles[UsersModel::FirstLetterRole] = "firstLetter";

    return roles;
}

void UsersModel::sortUsers()
{
    this->beginResetModel();

    std::sort(this->_users.begin(), this->_users.end(), [](User* usr1, User* usr2) {
        return TelegramHelper::fullName(usr1) < TelegramHelper::fullName(usr2);
    });

    this->endResetModel();
}

void UsersModel::telegramReady()
{
    this->_users = TelegramCache_users.values();
    this->sortUsers();
}

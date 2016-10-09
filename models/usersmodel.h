#ifndef USERSMODEL_H
#define USERSMODEL_H

#include "abstract/telegrammodel.h"

class UsersModel : public TelegramModel
{
    Q_OBJECT

    public:
        enum UserRoles { FirstLetterRole = Qt::UserRole + 10 };

    public:
        explicit UsersModel(QObject *parent = 0);
        virtual QVariant data(const QModelIndex &index, int role) const;
        virtual int rowCount(const QModelIndex &) const;
        virtual QHash<int, QByteArray> roleNames() const;

    protected:
        void sortUsers();
        void telegramReady();

    private:
        QList<User*> _users;
};

#endif // USERSMODEL_H

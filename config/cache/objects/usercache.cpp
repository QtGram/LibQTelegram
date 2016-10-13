#include "usercache.h"
#include "../../../types/telegramhelper.h"
#include <QDir>

UserCache::UserCache(QObject *parent) : AbstractCache("users", parent)
{
}

const QHash<TLInt, User *> &UserCache::users() const
{
    return this->_users;
}

User *UserCache::user(TLInt userid)
{
    if(!this->_users.contains(userid))
    {
        this->loadId(userid); // Try load...

        if(!this->_users.contains(userid))
        {
            qWarning("Cannot recover user %x from cache", userid);
            return NULL;
        }
    }

    return this->_users[userid];
}

void UserCache::cache(const TLVector<User *> &users)
{
    foreach(User* user, users)
        this->cache(user);
}

void UserCache::cache(User *user)
{
    TLInt userid = TelegramHelper::identifier(user);

    if(this->_users.contains(userid))
        return;

    user->setParent(this);
    this->_users[userid] = user;
}

void UserCache::save() const
{
    QList<TLInt> userids = this->_users.keys();

    foreach(TLInt userid, userids)
        this->saveId(userid);
}

void UserCache::doLoadId(TLInt id, MTProtoStream *mtstream)
{
    if(this->_users.contains(id))
        return;

    User* user = new User(this);
    user->read(mtstream);

    this->_users[id] = user;
}

void UserCache::doSaveId(TLInt id, MTProtoStream *mtstream) const
{
    if(!this->_users.contains(id))
        return;

    User* user = this->_users[id];
    user->write(mtstream);
}

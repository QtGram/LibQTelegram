#ifndef USERCACHE_H
#define USERCACHE_H

#include <QObject>
#include <QHash>
#include "../../telegramconfig.h"
#include "abstractcache.h"

class UserCache : public AbstractCache
{
    Q_OBJECT

    public:
        explicit UserCache(QObject *parent = 0);
        const QHash<TLInt, User*>& users() const;
        User* user(TLInt userid);
        void cache(const TLVector<User*>& users);
        void cache(User* user);
        void save() const;

   protected:
        virtual void doLoadId(TLInt id, MTProtoStream *mtstream);
        virtual void doSaveId(TLInt id, MTProtoStream *mtstream) const;

    private:
        QHash<TLInt, User*> _users;
};

#endif // USERCACHE_H

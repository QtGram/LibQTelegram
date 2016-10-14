#ifndef ABSTRACTCACHE_H
#define ABSTRACTCACHE_H

#include <functional>
#include <QObject>
#include "../../../mtproto/mtprotostream.h"

class AbstractCache : public QObject
{
    Q_OBJECT

    public:
        explicit AbstractCache(const QString& storagefolder, QObject *parent = 0);

    protected:
        void loadId(TLInt id);
        void saveId(TLInt id) const;
        void deleteId(TLInt id) const;
        void loadFile(const QString& filename, std::function<void(MTProtoStream*)> loadproc);
        void saveFile(const QString& filename, std::function<void(MTProtoStream*)> saveproc) const;
        virtual void doLoadId(TLInt id, MTProtoStream* mtstream) = 0;
        virtual void doSaveId(TLInt id, MTProtoStream* mtstream) const = 0;

    protected:
        QString _storagepath;
};

#endif // ABSTRACTCACHE_H

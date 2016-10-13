#include "abstractcache.h"
#include "../../telegramconfig.h"
#include "../../../types/telegramhelper.h"
#include <QFile>
#include <QDir>

AbstractCache::AbstractCache(const QString &storagefolder, QObject *parent): QObject(parent)
{
    this->_storagepath = TelegramConfig_storagePath + QDir::separator() + storagefolder;

    QDir dir;
    dir.mkpath(this->_storagepath);
}

void AbstractCache::loadId(TLInt id)
{
    this->loadFile(QString::number(id), [id, this](MTProtoStream* mtstream) {
        this->doLoadId(id, mtstream);
    });
}

void AbstractCache::saveId(TLInt id) const
{
    this->saveFile(QString::number(id), [id, this](MTProtoStream* mtstream) {
        this->doSaveId(id, mtstream);
    });
}

void AbstractCache::loadFile(const QString &filename, std::function<void (MTProtoStream *)> loadproc)
{
    QDir dir(this->_storagepath);
    MTProtoStream mtstream;

    mtstream.load(dir.absoluteFilePath(filename + ".cache"));
    loadproc(&mtstream);
}

void AbstractCache::saveFile(const QString &filename, std::function<void (MTProtoStream *)> saveproc) const
{
    QDir dir(this->_storagepath);
    MTProtoStream mtstream;

    saveproc(&mtstream);
    mtstream.save(dir.absoluteFilePath(filename + ".cache"));
}

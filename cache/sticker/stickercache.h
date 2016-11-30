#ifndef STICKERCACHE_H
#define STICKERCACHE_H

#define StickerCache_instance StickerCache::instance()
#define StickerCache_stickerPreview(stickerset) StickerCache::instance()->stickerPreview(stickerset)
#define StickerCache_populateInstalled(stickersets) StickerCache::instance()->populateInstalled(stickersets)
#define StickerCache_populatePack(stickerset, stickers) StickerCache::instance()->populatePack(stickerset, stickers);
#define StickerCache_populate(allstickers) StickerCache::instance()->populate(allstickers);

#include <QObject>
#include <QHash>
#include "../../mtproto/mtprotoupdatehandler.h"
#include "../database/cachedatabase.h"

class StickerCache : public QObject
{
    Q_OBJECT

    private:
        explicit StickerCache(CacheDatabase* cachedatabase, QObject *parent = 0);

    public:
        static StickerCache* instance();
        static void init(CacheDatabase* cachedatabase);
        Document* stickerPreview(StickerSet* stickerset);
        void populate(MessagesAllStickers* messageallstickers);
        void populateInstalled(QList<StickerSet*>& stickerssets);
        void populatePack(StickerSet* stickerset, QList<Document*>& stickers);

    private:
        MessagesStickerSet* stickerSetData(StickerSet* stickerset);
        void populateStickerData();

    private slots:
        void onStickerSetDataReceived(MTProtoReply* mtreply);

    signals:
        void stickerCacheUpdated();

    private:
        bool _updating;
        CacheDatabase* _cachedatabase;
        QHash<TLLong, StickerSet*> _stickersets;
        QHash<TLLong, MessagesStickerSet*> _stickersetsdata;
        QList<StickerSet*> _stickers;
        QList<StickerSet*> _pendingdata;

    private:
        static StickerCache* _instance;
};

#endif // STICKERCACHE_H

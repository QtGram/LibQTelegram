#include "stickercache.h"
#include "../../mtproto/dc/dcsessionmanager.h"

StickerCache* StickerCache::_instance = NULL;

StickerCache::StickerCache(CacheDatabase *cachedatabase, QObject *parent) : QObject(parent), _updating(false), _cachedatabase(cachedatabase)
{
    cachedatabase->stickerSets()->populate(this->_stickersets, this);
}

StickerCache *StickerCache::instance()
{
    return StickerCache::_instance;
}

void StickerCache::init(CacheDatabase *cachedatabase)
{
    if(StickerCache::_instance)
        return;

    StickerCache::_instance = new StickerCache(cachedatabase);
}

Document *StickerCache::stickerPreview(StickerSet *stickerset)
{
    if(this->_updating || (stickerset->count() <= 0))
        return NULL;

    MessagesStickerSet* messagesstickerset = this->stickerSetData(stickerset);

    if(!messagesstickerset)
        return NULL;

    return messagesstickerset->documents().first();
}

void StickerCache::populate(MessagesAllStickers *messageallstickers)
{
    Q_ASSERT((messageallstickers->constructorId() == TLTypes::MessagesAllStickersNotModified) ||
             (messageallstickers->constructorId() == TLTypes::MessagesAllStickers));

    if(messageallstickers->constructorId() == TLTypes::MessagesAllStickersNotModified)
        return;

    this->_updating = true;

    this->_cachedatabase->transaction([this, messageallstickers](QSqlQuery& queryobj) {
        this->_cachedatabase->stickerSets()->prepareInsert(queryobj);

        foreach(StickerSet* stickerset, messageallstickers->sets()) {
            stickerset->setParent(this);

            this->_pendingdata << stickerset;
            this->_stickers << stickerset;

            this->_stickersets[stickerset->id()] = stickerset;
            this->_cachedatabase->stickerSets()->insertQuery(queryobj, stickerset);
        }

    });

    this->_cachedatabase->transaction([this, messageallstickers](QSqlQuery& queryobj) {
        int index = 0;
        this->_cachedatabase->stickerSets()->prepareOrder(queryobj);

        foreach(StickerSet* stickerset, messageallstickers->sets())
            this->_cachedatabase->stickerSets()->orderQuery(queryobj, stickerset, index++);

        this->populateStickerData();
    });
}

void StickerCache::populateInstalled(QList<StickerSet *> &stickerssets)
{
    if(this->_updating)
        return;

    stickerssets.clear();

    foreach(StickerSet* stickerset, this->_stickers)
    {
        if(!stickerset->isInstalled() || stickerset->isArchived())
            continue;

        stickerssets << stickerset;
    }
}

void StickerCache::populatePack(StickerSet *stickerset, QList<Document *> &stickers)
{
    MessagesStickerSet* messagesstickerset = this->stickerSetData(stickerset);

    if(!messagesstickerset)
    {
        stickers.clear();
        return;
    }

    stickers = messagesstickerset->documents();
}

MessagesStickerSet *StickerCache::stickerSetData(StickerSet *stickerset)
{
    if(!this->_stickersetsdata.contains(stickerset->id()))
    {
        MessagesStickerSet* messagesstickerset = this->_cachedatabase->stickerSetsData()->get<MessagesStickerSet>(stickerset->id(), "stickersetdata", false, this);

        if(!messagesstickerset)
            return NULL;

        this->_stickersetsdata[stickerset->id()] = messagesstickerset;
    }

    return this->_stickersetsdata[stickerset->id()];
}

void StickerCache::populateStickerData()
{
    if(this->_pendingdata.isEmpty())
    {
        this->_updating = false;
        emit stickerCacheUpdated();
        return;
    }

    StickerSet* stickerset = this->_pendingdata.takeFirst();
    InputStickerSet* inputstickerset = TelegramHelper::inputStickerSet(stickerset);

    MTProtoRequest* req = TelegramAPI::messagesGetStickerSet(DC_MainSession, inputstickerset);
    connect(req, &MTProtoRequest::replied, this, &StickerCache::onStickerSetDataReceived);
    inputstickerset->deleteLater();
}

void StickerCache::onStickerSetDataReceived(MTProtoReply *mtreply)
{
    MessagesStickerSet* messagesstickerset = new MessagesStickerSet(this);
    messagesstickerset->read(mtreply);

    this->_stickersetsdata[messagesstickerset->set()->id()] = messagesstickerset;
    this->_cachedatabase->stickerSetsData()->insert(messagesstickerset);
    this->populateStickerData();
}

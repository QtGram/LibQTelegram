#include "qquickmediamessageitem.h"
#include <QDateTime>

QQuickMediaMessageItem::QQuickMediaMessageItem(QQuickItem *parent): QQuickBaseItem(parent), _message(NULL), _size(0), _contentwidth(0)
{
    this->_imagecomponent = NULL;
    this->_animatedcomponent = NULL;
    this->_locationcomponent = NULL;
    this->_webpagecomponent = NULL;
    this->_audiocomponent = NULL;
    this->_filecomponent = NULL;

    connect(this, &QQuickMediaMessageItem::messageChanged, this, &QQuickMediaMessageItem::isStickerChanged);
    connect(this, &QQuickMediaMessageItem::messageChanged, this, &QQuickMediaMessageItem::isAnimatedChanged);
    connect(this, &QQuickMediaMessageItem::messageChanged, this, &QQuickMediaMessageItem::isAudioChanged);
    connect(this, &QQuickMediaMessageItem::messageChanged, this, &QQuickMediaMessageItem::isImageChanged);
    connect(this, &QQuickMediaMessageItem::messageChanged, this, &QQuickMediaMessageItem::isWebPageChanged);
}

Message *QQuickMediaMessageItem::message() const
{
    return this->_message;
}

bool QQuickMediaMessageItem::isSticker() const
{
    if(!this->_message || !this->_message->media())
        return false;

    MessageMedia* messagemedia = this->_message->media();

    if(messagemedia->constructorId() != TLTypes::MessageMediaDocument)
        return false;

    return TelegramHelper::documentHas(messagemedia->document(), TLTypes::DocumentAttributeSticker) != NULL;
}

bool QQuickMediaMessageItem::isAnimated() const
{
    if(!this->_message || !this->_message->media())
        return false;

    MessageMedia* messagemedia = this->_message->media();

    if(messagemedia->constructorId() != TLTypes::MessageMediaDocument)
        return false;

    return TelegramHelper::documentHas(messagemedia->document(), TLTypes::DocumentAttributeAnimated) != NULL;
}

bool QQuickMediaMessageItem::isAudio() const
{
    if(!this->_message || !this->_message->media())
        return false;

    MessageMedia* messagemedia = this->_message->media();

    if(messagemedia->constructorId() != TLTypes::MessageMediaDocument)
        return false;

    return TelegramHelper::documentHas(messagemedia->document(), TLTypes::DocumentAttributeAudio) != NULL;
}

bool QQuickMediaMessageItem::isVideo() const
{
    if(!this->_message || !this->_message->media())
        return false;

    MessageMedia* messagemedia = this->_message->media();

    if(messagemedia->constructorId() != TLTypes::MessageMediaDocument)
        return false;

    return TelegramHelper::documentHas(messagemedia->document(), TLTypes::DocumentAttributeVideo) != NULL;
}

bool QQuickMediaMessageItem::isImage() const
{
    if(!this->_message || !this->_message->media())
        return false;

    MessageMedia* messagemedia = this->_message->media();

    if(messagemedia->constructorId() != TLTypes::MessageMediaDocument)
        return false;

    return TelegramHelper::documentHas(messagemedia->document(), TLTypes::DocumentAttributeImageSize) != NULL;
}

bool QQuickMediaMessageItem::isWebPage() const
{
    if(!this->_message || !this->_message->media())
        return false;

    MessageMedia* messagemedia = this->_message->media();
    return messagemedia->constructorId() != TLTypes::MessageMediaWebPage;
}

qreal QQuickMediaMessageItem::size() const
{
    return this->_size;
}

qreal QQuickMediaMessageItem::contentWidth() const
{
    return this->_contentwidth;
}

QString QQuickMediaMessageItem::duration() const
{
    if(!this->_message)
        return QString();

    MessageMedia* messagemedia = this->_message->media();

    if(!messagemedia || (messagemedia->constructorId() != TLTypes::MessageMediaDocument))
        return QString();

    DocumentAttribute* attribute = NULL;

    if((attribute = TelegramHelper::documentHas(messagemedia->document(), TLTypes::DocumentAttributeAudio)))
        return TelegramHelper::duration(attribute->duration());

    return QString();
}

QString QQuickMediaMessageItem::venueTitle() const
{
    if(!this->_message)
        return QString();

    MessageMedia* messagemedia = this->_message->media();

    if(!messagemedia || (messagemedia->constructorId() != TLTypes::MessageMediaVenue))
        return QString();

    return messagemedia->title();
}

QString QQuickMediaMessageItem::venueAddress() const
{
    if(!this->_message)
        return QString();

    MessageMedia* messagemedia = this->_message->media();

    if(!messagemedia || (messagemedia->constructorId() != TLTypes::MessageMediaVenue))
        return QString();

    return messagemedia->address();
}

QString QQuickMediaMessageItem::webPageTitle() const
{
    if(!this->_message)
        return NULL;

    MessageMedia* messagemedia = this->_message->media();

    if(!messagemedia || (messagemedia->constructorId() != TLTypes::MessageMediaWebPage))
        return NULL;

    if(!messagemedia->webpage()->title().isEmpty())
        return messagemedia->webpage()->title();

    if(!messagemedia->webpage()->author().isEmpty())
        return messagemedia->webpage()->siteName() + "\n" + messagemedia->webpage()->author();

    return messagemedia->webpage()->siteName();
}

QString QQuickMediaMessageItem::webPageDescription() const
{
    if(!this->_message)
        return NULL;

    MessageMedia* messagemedia = this->_message->media();

    if(!messagemedia || (messagemedia->constructorId() != TLTypes::MessageMediaWebPage))
        return NULL;

    return messagemedia->webpage()->description();
}

QString QQuickMediaMessageItem::webPageUrl() const
{
    if(!this->_message)
        return NULL;

    MessageMedia* messagemedia = this->_message->media();

    if(!messagemedia || (messagemedia->constructorId() != TLTypes::MessageMediaWebPage))
        return NULL;

    return messagemedia->webpage()->url();
}

bool QQuickMediaMessageItem::webPageHasPhoto() const
{
    if(!this->_message)
        return false;

    MessageMedia* messagemedia = this->_message->media();

    if(!messagemedia || (messagemedia->constructorId() != TLTypes::MessageMediaWebPage))
        return false;

    return messagemedia->webpage()->photo() && (messagemedia->webpage()->photo()->constructorId() == TLTypes::Photo);
}

GeoPoint *QQuickMediaMessageItem::geoPoint() const
{
    if(!this->_message)
        return NULL;

    MessageMedia* messagemedia = this->_message->media();

    if(!messagemedia || ((messagemedia->constructorId() != TLTypes::MessageMediaGeo) && (messagemedia->constructorId() != TLTypes::MessageMediaVenue)))
        return NULL;

    return messagemedia->geo();
}

void QQuickMediaMessageItem::setMessage(Message *message)
{
    if(this->_message == message)
        return;

    this->_message = message;
    emit messageChanged();
}

void QQuickMediaMessageItem::setSize(qreal size)
{
    if(this->_size == size)
        return;

    this->_size = size;
    emit sizeChanged();
}

QQmlComponent *QQuickMediaMessageItem::imageDelegate() const
{
    return this->_imagecomponent;
}

void QQuickMediaMessageItem::setImageDelegate(QQmlComponent *imagecomponent)
{
    if(this->_imagecomponent == imagecomponent)
        return;

    this->_imagecomponent = imagecomponent;
    emit imageDelegateChanged();
}

QQmlComponent *QQuickMediaMessageItem::animatedDelegate() const
{
    return this->_animatedcomponent;
}

void QQuickMediaMessageItem::setAnimatedDelegate(QQmlComponent *animatedcomponent)
{
    if(this->_animatedcomponent == animatedcomponent)
        return;

    this->_animatedcomponent = animatedcomponent;
    emit animatedDelegateChanged();
}

QQmlComponent *QQuickMediaMessageItem::locationDelegate() const
{
    return this->_locationcomponent;
}

void QQuickMediaMessageItem::setLocationDelegate(QQmlComponent *locationcomponent)
{
    if(this->_locationcomponent == locationcomponent)
        return;

    this->_locationcomponent = locationcomponent;
    emit locationDelegateChanged();
}

QQmlComponent *QQuickMediaMessageItem::webPageDelegate() const
{
    return this->_webpagecomponent;
}

void QQuickMediaMessageItem::setWebPageDelegate(QQmlComponent *webpagecomponent)
{
    if(this->_webpagecomponent == webpagecomponent)
        return;

    this->_webpagecomponent = webpagecomponent;
    emit webPageDelegateChanged();
}

QQmlComponent *QQuickMediaMessageItem::audioDelegate() const
{
    return this->_audiocomponent;
}

void QQuickMediaMessageItem::setAudioDelegate(QQmlComponent *audiocomponent)
{
    if(this->_audiocomponent == audiocomponent)
        return;

    this->_audiocomponent = audiocomponent;
    emit audioDelegateChanged();
}

QQmlComponent *QQuickMediaMessageItem::fileDelegate() const
{
    return this->_filecomponent;
}

void QQuickMediaMessageItem::setFileDelegate(QQmlComponent *filecomponent)
{
    if(this->_filecomponent == filecomponent)
        return;

    this->_filecomponent = filecomponent;
    emit fileDelegateChanged();
}

void QQuickMediaMessageItem::download()
{
    if(!this->canDownload())
        return;

    QQuickBaseItem::download();
}

void QQuickMediaMessageItem::updateContentWidth(qreal contentwidth)
{
    if(this->_contentwidth == contentwidth)
        return;

    this->_contentwidth = contentwidth;
    emit contentWidthChanged();
}

void QQuickMediaMessageItem::updateContentWidth()
{
    QSize imagesize = this->imageSize();

    if(!imagesize.isValid() || (this->_contentwidth == imagesize.width()))
        return;

    this->_contentwidth = imagesize.width();
    emit contentWidthChanged();
}

qreal QQuickMediaMessageItem::calcAspectRatio(const QSize& imagesize) const
{
    if(!imagesize.isValid() || !imagesize.height())
        return 1.0;

    return imagesize.width() / static_cast<qreal>(imagesize.height());
}

bool QQuickMediaMessageItem::canDownload() const
{
    if(!this->_message || !this->_message->media())
        return false;

    switch(this->_message->media()->constructorId())
    {
        case TLTypes::MessageMediaPhoto:
        case TLTypes::MessageMediaDocument:
            return true;

        default:
            break;
    }

    return false;
}

void QQuickMediaMessageItem::createImageElement()
{
    if(!this->_imagecomponent)
        return;

    this->createObject(this->_imagecomponent);
    connect(this, &QQuickMediaMessageItem::sizeChanged, this, &QQuickMediaMessageItem::scaleToImageSize);
    this->scaleToImageSize();
}

void QQuickMediaMessageItem::createAnimatedElement()
{
    if(!this->_animatedcomponent)
        return;

    this->createObject(this->_animatedcomponent);
    connect(this, &QQuickMediaMessageItem::sizeChanged, this, &QQuickMediaMessageItem::scaleToImageSize);
    this->scaleToImageSize();
}

void QQuickMediaMessageItem::createLocationElement()
{
    if(!this->_locationcomponent)
        return;

    this->createObject(this->_locationcomponent);
    connect(this->_mediaelement, &QQuickItem::widthChanged, this, &QQuickMediaMessageItem::scaleToImage);
    connect(this->_mediaelement, &QQuickItem::heightChanged, this, &QQuickMediaMessageItem::scaleToImage);

    if(this->_message->media()->constructorId() == TLTypes::MessageMediaVenue)
    {
        emit venueTitleChanged();
        emit venueAddressChanged();
    }

    emit geoPointChanged();
    this->scaleToImage();
}

void QQuickMediaMessageItem::createWebPageElement()
{
    if(!this->_webpagecomponent)
        return;

    this->createObject(this->_webpagecomponent);
    connect(this->_mediaelement, &QQuickItem::heightChanged, this, &QQuickMediaMessageItem::scaleToFree);
    connect(this->_mediaelement, &QQuickItem::widthChanged, this, &QQuickMediaMessageItem::scaleToFree);
    connect(this, &QQuickMediaMessageItem::sizeChanged, this, &QQuickMediaMessageItem::scaleToFree);

    emit webPageTitleChanged();
    emit webPageDescriptionChanged();
    emit webPageUrlChanged();
    emit webPageHasPhotoChanged();
    emit isWebPageChanged();

    this->scaleToFree();
}

void QQuickMediaMessageItem::createAudioElement()
{
    if(!this->_audiocomponent)
        return;

    this->createObject(this->_audiocomponent);
    connect(this->_mediaelement, &QQuickItem::heightChanged, this, &QQuickMediaMessageItem::scaleToFree);
    connect(this->_mediaelement, &QQuickItem::widthChanged, this, &QQuickMediaMessageItem::scaleToFree);
    connect(this, &QQuickMediaMessageItem::sizeChanged, this, &QQuickMediaMessageItem::scaleToFree);

    emit durationChanged();
    this->scaleToFree();
}

void QQuickMediaMessageItem::createFileElement()
{
    if(!this->_filecomponent)
        return;

    this->createObject(this->_filecomponent);
    connect(this->_mediaelement, &QQuickItem::heightChanged, this, &QQuickMediaMessageItem::scaleToFree);
    connect(this->_mediaelement, &QQuickItem::widthChanged, this, &QQuickMediaMessageItem::scaleToFree);
    connect(this, &QQuickMediaMessageItem::sizeChanged, this, &QQuickMediaMessageItem::scaleToFree);

    emit fileNameChanged();
    emit fileSizeChanged();

    this->scaleToFree();
}

void QQuickMediaMessageItem::initialize()
{
    if(!this->_message || this->_mediaelement)
        return;

    FileObject* fileobject = this->createFileObject(this->_message);

    if(fileobject)
    {
        connect(fileobject, SIGNAL(imageSizeChanged()), this, SLOT(updateContentWidth()));
        this->updateContentWidth();
    }

    MessageMedia* messagemedia = this->_message->media();

    if(!messagemedia || (messagemedia->constructorId() == TLTypes::MessageMediaEmpty))
    {
        this->setVisible(false);
        return;
    }
    else if(messagemedia->constructorId() == TLTypes::MessageMediaWebPage)
    {
        if(TelegramHelper::messageIsWebPagePending(this->_message))
        {
            connect(messagemedia, &MessageMedia::webpageChanged, this, &QQuickMediaMessageItem::initialize);
            return;
        }

        disconnect(messagemedia, &MessageMedia::webpageChanged, this, 0);
    }

    switch(messagemedia->constructorId())
    {
        case TLTypes::MessageMediaPhoto:
            this->createImageElement();
            break;

        case TLTypes::MessageMediaGeo:
        case TLTypes::MessageMediaVenue:
            this->createLocationElement();
            break;

        case TLTypes::MessageMediaDocument:
        {
            Document* document = messagemedia->document();

            if(TelegramHelper::isSticker(document))
                this->createImageElement();
            else if(TelegramHelper::isAnimated(document))
                this->createAnimatedElement();
            else if(TelegramHelper::isVideo(document))
                this->createImageElement();
            else if(TelegramHelper::isAudio(document))
                this->createAudioElement();
            else if(TelegramHelper::isFile(document))
                this->createFileElement();

            break;
        }

        case TLTypes::MessageMediaWebPage:
            this->createWebPageElement();
            break;

        default:
            qDebug() << "Unhandled Message Media Type:" << messagemedia->constructorId();
            return;
    }

    emit sourceChanged();
}

void QQuickMediaMessageItem::componentComplete()
{
    QQuickBaseItem::componentComplete();

    this->initialize();
}

void QQuickMediaMessageItem::scaleToImageSize()
{
    QSize imagesize = this->imageSize();

    if(!imagesize.isValid())
        return;

    qreal aspectratio = this->calcAspectRatio(imagesize);
    this->setWidth(this->_size);
    this->setHeight(this->_size / aspectratio);
}

void QQuickMediaMessageItem::scaleToImage()
{
    this->updateContentWidth(this->_mediaelement->width());
    this->setWidth(this->_size);
    this->setHeight(this->_mediaelement->height());
}

void QQuickMediaMessageItem::scaleToColumn()
{
    this->setWidth(this->_size);
    this->setHeight(this->_mediaelement->height());
}

void QQuickMediaMessageItem::scaleToFree()
{
    this->updateContentWidth(this->_mediaelement->width());
    this->setWidth(this->_mediaelement->width());
    this->setHeight(this->_mediaelement->height());
}

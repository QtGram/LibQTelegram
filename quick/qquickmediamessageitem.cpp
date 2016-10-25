#include "qquickmediamessageitem.h"
#include "../types/telegramhelper.h"
#include <QDateTime>

QQuickMediaMessageItem::QQuickMediaMessageItem(QQuickItem *parent): QQuickBaseItem(parent), _message(NULL), _size(0), _contentwidth(0)
{
    connect(this, &QQuickMediaMessageItem::messageChanged, this, &QQuickMediaMessageItem::isStickerChanged);
    connect(this, &QQuickMediaMessageItem::messageChanged, this, &QQuickMediaMessageItem::isAnimatedChanged);
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

    return TelegramHelper::isSticker(messagemedia->document());
}

bool QQuickMediaMessageItem::isAnimated() const
{
    if(!this->_message || !this->_message->media())
        return false;

    MessageMedia* messagemedia = this->_message->media();

    if(messagemedia->constructorId() != TLTypes::MessageMediaDocument)
        return false;

    return TelegramHelper::isAnimated(messagemedia->document());
}

qreal QQuickMediaMessageItem::size() const
{
    return this->_size;
}

qreal QQuickMediaMessageItem::contentWidth() const
{
    return this->_contentwidth;
}

const QJSValue &QQuickMediaMessageItem::locationDelegate() const
{
   return this->_locationdelegate;
}

void QQuickMediaMessageItem::setMessage(Message *message)
{
    if(this->_message == message)
        return;

    this->_message = message;
    this->initialize();
    emit messageChanged();
}

void QQuickMediaMessageItem::setSize(qreal size)
{
    if(this->_size == size)
        return;

    this->_size = size;
    emit sizeChanged();
}

void QQuickMediaMessageItem::setLocationDelegate(const QJSValue &locationdelegate)
{
    if(this->_locationdelegate.equals(locationdelegate))
        return;

    this->_locationdelegate = locationdelegate;
    this->callLocationDelegate();
    emit locationDelegateChanged();
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

void QQuickMediaMessageItem::createImageElement()
{
    this->createComponent("Image {\n"
                              "id: image\n"
                              "anchors.fill: parent\n"
                              "asynchronous: true\n"
                          "}");

    connect(this, &QQuickMediaMessageItem::sizeChanged, this, &QQuickMediaMessageItem::scaleToImageSize);
    this->scaleToImageSize();
}

void QQuickMediaMessageItem::createAnimatedElement()
{
    QString componentsource = "Item {\n"
                                  "property url source\n"
                                  "id: animatedelement\n"
                                  "anchors.fill: parent\n"
                                  "MediaPlayer {\n"
                                      "id: mediaplayer\n"
                                      "loops: MediaPlayer.Infinite\n"
                                      "autoPlay: true\n"
                                      "source: animatedelement.source\n"
                                  "}\n"
                                  "VideoOutput {\n"
                                      "id: videooutput\n"
                                      "anchors.fill: parent\n"
                                      "source: mediaplayer\n"
                                  "}\n"
                              "}";

    this->createComponent(componentsource);
    connect(this, &QQuickMediaMessageItem::sizeChanged, this, &QQuickMediaMessageItem::scaleToImageSize);
    this->scaleToImageSize();
}

void QQuickMediaMessageItem::createLocationElement()
{
    QString componentstring = "Column {\n"
                                  "property url source\n"
                                  "id: locationelement\n"
                                  "width: imgmap.width\n"
                                  "Text {\n"
                                      "id: txtvenue\n"
                                      "width: parent.width\n"
                                      "color: \"%1\"\n"
                                      "wrapMode: Text.Wrap\n"
                                      "font.pixelSize: %2\n"
                                      "visible: (text.length > 0)\n"
                                      "text: \"%3\"\n"
                                  "}\n"
                                  "Image {\n"
                                      "id: imgmap\n"
                                      "asynchronous: true\n"
                                      "fillMode: Image.PreserveAspectFit\n"
                                      "source: locationelement.source\n"
                                  "}\n"
                              "}";

    QString venue;

    if(this->_message->media()->constructorId() == TLTypes::MessageMediaVenue)
    {
        MessageMedia* messagemedia = this->_message->media();
        venue = messagemedia->title() + "\n" + messagemedia->address();
    }

    this->createComponent(componentstring.arg(this->foregroundColor().name())
                                         .arg(this->fontPixelSize())
                                         .arg(this->escape(venue)));

    connect(this->_mediaelement, &QQuickItem::widthChanged, this, &QQuickMediaMessageItem::scaleToImage);
    connect(this->_mediaelement, &QQuickItem::heightChanged, this, &QQuickMediaMessageItem::scaleToImage);
    this->scaleToImage();
}

void QQuickMediaMessageItem::createWebPagePhotoElement()
{
    QString componentstring = "Column {\n"
                                  "property url source\n"
                                  "id: webpagephotoelement\n"
                                  "width: parent.width\n"
                              "Text {\n"
                                  "id: wpurl\n"
                                  "width: parent.width\n"
                                  "wrapMode: Text.Wrap\n"
                                  "text: \"%1\"\n"
                              "}\n"
                              "Text {\n"
                                  "id: wptitle\n"
                                  "width: parent.width\n"
                                  "wrapMode: Text.NoWrap\n"
                                  "elide: Text.ElideRight\n"
                                  "text: \"%2\"\n"
                              "}\n"
                              "Text {\n"
                                  "id: wpdescription\n"
                                  "width: parent.width\n"
                                  "wrapMode: Text.Wrap\n"
                                  "text: \"%3\"\n"
                              "}\n"
                              "Image {\n"
                                  "width: parent.width\n"
                                  "asynchronous: true\n"
                                  "fillMode: Image.PreserveAspectFit\n"
                                  "source: webpagephotoelement.source\n"
                              "}\n"
                            "}";

    WebPage* webpage = this->_message->media()->webpage();

    this->createComponent(componentstring.arg(this->escape(webpage->url()),
                                              this->escape(webpage->title()),
                                              this->escape(webpage->description())));

    connect(this->_mediaelement, &QQuickItem::heightChanged, this, &QQuickMediaMessageItem::scaleToColumn);
    connect(this, &QQuickMediaMessageItem::sizeChanged, this, &QQuickMediaMessageItem::scaleToColumn);
    this->scaleToColumn();
}

void QQuickMediaMessageItem::createFileElement()
{
    QString componentstring = "Row {\n"
                                  "property url source\n"
                                  "id: fileelement\n"
                                  "width: parent.width\n"
                                  "height: textcolumn.height\n"
                                  "Image {\n"
                                      "id: image\n"
                                      "height: textcolumn.height\n"
                                      "width: height\n"
                                      "asynchronous: true\n"
                                  "}\n"
                                  "Column {\n"
                                      "id: textcolumn\n"
                                      "width: parent.width - x\n"
                                      "height: childrenRect.height\n"
                                      "Text {\n"
                                          "width: parent.width\n"
                                          "text: \"%1\"\n"
                                      "}\n"
                                      "Text {\n"
                                          "width: parent.width\n"
                                          "text: \"%2\"\n"
                                      "}\n"
                                  "}\n"
                              "}";

    MessageMedia* messagemedia = this->_message->media();

    TLString fileName;
    foreach(DocumentAttribute* attribute, messagemedia->document()->attributes())
    {
        if(attribute->constructorId() == TLTypes::DocumentAttributeFilename)
            fileName = attribute->fileName();
    }
    QString fileSize;
    TLDouble size = messagemedia->document()->size();
    int unit = 0;
    while (size > 1024) {
        size /= 1024;
        unit++;
    }
    fileSize = QString::number(size, 'g', 2);
    switch (unit) {
    case 0:
        fileSize += "B";
        break;
    case 1:
        fileSize += "KB";
        break;
    case 2:
        fileSize += "MB";
        break;
    case 3:
        fileSize += "GB";
        break;
    }

    this->createComponent(componentstring.arg(this->escape(fileName),
                                              this->escape(fileSize)));

    connect(this->_mediaelement, &QQuickItem::heightChanged, this, &QQuickMediaMessageItem::scaleToColumn);
    connect(this->_mediaelement, &QQuickItem::widthChanged, this, &QQuickMediaMessageItem::scaleToColumn);
    connect(this, &QQuickMediaMessageItem::sizeChanged, this, &QQuickMediaMessageItem::scaleToColumn);
    this->scaleToColumn();
}

void QQuickMediaMessageItem::initialize()
{
    if(!this->_message || this->_mediaelement)
        return;

    MessageMedia* messagemedia = this->_message->media();

    if(!messagemedia || (messagemedia->constructorId() == TLTypes::MessageMediaEmpty))
    {
        this->setVisible(false);
        return;
    }

    switch(messagemedia->constructorId())
    {
        case TLTypes::MessageMediaPhoto:
            this->createImageElement();
            break;

        case TLTypes::MessageMediaGeo:
        case TLTypes::MessageMediaVenue:
            this->createLocationElement();
            this->callLocationDelegate();
            break;

        case TLTypes::MessageMediaDocument:
        {
            Document* document = messagemedia->document();

            if(TelegramHelper::isSticker(document))
                this->createImageElement();
            else if(TelegramHelper::isAnimated(document))
                this->createAnimatedElement();
            else if(TelegramHelper::isFile(document))
                this->createFileElement();

            break;
        }

        case TLTypes::MessageMediaWebPage:
        {
            WebPage* webpage = messagemedia->webpage();

            if(webpage->photo())
                this->createWebPagePhotoElement();
            else
                return;

            break;
        }

        default:
            qDebug() << "Unhandled Message Media Type:" << messagemedia->constructorId();
            return;
    }

    FileObject* fileobject = this->createFileObject(this->_message);

    if(!fileobject)
        return;

    connect(fileobject, SIGNAL(imageSizeChanged()), this, SLOT(updateContentWidth()));
    this->updateContentWidth();
    this->bindToElement();
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

void QQuickMediaMessageItem::callLocationDelegate()
{
    if(!this->_message || !this->_message->media() || !this->_mediaelement || this->_locationdelegate.isNull() || !this->_locationdelegate.isCallable())
        return;

    if((this->_message->media()->constructorId() != TLTypes::MessageMediaGeo) && (this->_message->media()->constructorId() != TLTypes::MessageMediaVenue))
        return;

    GeoPoint* geopoint = this->_message->media()->geo();

    QJSValueList args;
    args << geopoint->latitude() << geopoint->longitude();
    QJSValue result = this->_locationdelegate.call(args);

    if(!result.isString())
        return;

    this->updateSource(result.toVariant());
}

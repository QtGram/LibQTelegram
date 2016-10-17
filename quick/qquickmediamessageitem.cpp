#include "qquickmediamessageitem.h"
#include <QDateTime>

QQuickMediaMessageItem::QQuickMediaMessageItem(QQuickItem *parent): QQuickBaseItem(parent), _message(NULL), _size(0)
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

    return this->documentIsSticker(messagemedia->document());
}

bool QQuickMediaMessageItem::isAnimated() const
{
    if(!this->_message || !this->_message->media())
        return false;

    MessageMedia* messagemedia = this->_message->media();

    if(messagemedia->constructorId() != TLTypes::MessageMediaDocument)
        return false;

    return this->documentIsAnimated(messagemedia->document());
}

int QQuickMediaMessageItem::size() const
{
    return this->_size;
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

void QQuickMediaMessageItem::setSize(int size)
{
    if(this->_size == size)
        return;

    this->_size = size;
    this->updateMetrics();
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

bool QQuickMediaMessageItem::documentIsSticker(Document *document) const
{
    foreach(DocumentAttribute* attribute, document->attributes())
    {
        if(attribute->constructorId() == TLTypes::DocumentAttributeSticker)
            return true;
    }

    return false;
}

bool QQuickMediaMessageItem::documentIsAnimated(Document *document) const
{
    foreach(DocumentAttribute* attribute, document->attributes())
    {
        if(attribute->constructorId() == TLTypes::DocumentAttributeAnimated)
            return true;
    }

    return false;
}

void QQuickMediaMessageItem::createLocationElement()
{
    QString componentstring = "Item {\n"
                                  "property url source\n"
                                  "id: locationelement\n"
                                  "width: imgmap.width\n"
                                  "height: imgmap.height + (txtvenue.visible ? txtvenue.contentHeight : 0)\n"
                                  "Text {\n"
                                      "id: txtvenue\n"
                                      "anchors { left: parent.left; top: parent.top; right: parent.right }\n"
                                      "color: \"%1\"\n"
                                      "wrapMode: Text.Wrap\n"
                                      "font.pixelSize: %2\n"
                                      "visible: (text.length > 0)\n"
                                      "text: \"%3\"\n"
                                  "}\n"
                                  "Image {\n"
                                      "id: imgmap\n"
                                      "anchors { left: parent.left; top: txtvenue.bottom }\n"
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
                                         .arg(venue));
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

    if(messagemedia->constructorId() == TLTypes::MessageMediaPhoto)
    {
        this->createImageElement();
    }
    else if((messagemedia->constructorId() == TLTypes::MessageMediaGeo) || (messagemedia->constructorId() == TLTypes::MessageMediaVenue))
    {
        this->createLocationElement();
        this->callLocationDelegate();
    }
    else if(messagemedia->constructorId() == TLTypes::MessageMediaDocument)
    {
        Document* document = messagemedia->document();

        if(this->documentIsSticker(document))
            this->createImageElement();
        else if(this->documentIsAnimated(document))
            this->createAnimatedElement();
    }
    else
        return;

    FileObject* fileobject = this->createFileObject(this->_message);

    if(!fileobject)
        return;

    connect(fileobject, &FileObject::imageSizeChanged, this, &QQuickMediaMessageItem::updateMetrics);
    this->updateMetrics();
    this->bindToElement();
}

void QQuickMediaMessageItem::updateMetrics()
{
    if(!this->_size)
        return;

    QSize imagesize = this->imageSize();

    if(!imagesize.isValid())
        return;

    qreal aspectratio = imagesize.width() / static_cast<qreal>(imagesize.height());

    this->setWidth(this->_size);
    this->setHeight(aspectratio ? (this->_size / aspectratio) : 0);
}

void QQuickMediaMessageItem::callLocationDelegate()
{
    if(!this->_message || !this->_message->media() || !this->_mediaelement || this->_locationdelegate.isNull() || !this->_locationdelegate.isCallable())
        return;

    if((this->_message->media()->constructorId() != TLTypes::MessageMediaGeo) && (this->_message->media()->constructorId() != TLTypes::MessageMediaVenue))
        return;

    GeoPoint* geopoint = this->_message->media()->geo();

    QJSValueList args;
    args <<  geopoint->latitude() << geopoint->longitude();
    QJSValue result = this->_locationdelegate.call(args);

    if(!result.isString())
        return;

    this->updateSource(result.toVariant());
}

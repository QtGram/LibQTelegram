#include "qquickbaseitem.h"
#include <QCryptographicHash>

QHash<QByteArray, QQmlComponent*> QQuickBaseItem::_components;

QQuickBaseItem::QQuickBaseItem(QQuickItem *parent) : QQuickItem(parent), _fileobject(NULL), _mediaelement(NULL)
{
    this->_version = "2.0"; // Target base version by default
}

QString QQuickBaseItem::version() const
{
    return this->_version;
}

QSize QQuickBaseItem::imageSize() const
{
    if(!this->_fileobject)
        return QSize();

    return this->_fileobject->imageSize();
}

bool QQuickBaseItem::downloaded() const
{
    if(!this->_fileobject)
        return false;

    return this->_fileobject->downloaded();
}

bool QQuickBaseItem::downloading() const
{
    if(!this->_fileobject)
        return false;

    return this->_fileobject->downloading();
}

bool QQuickBaseItem::hasThumbnail() const
{
    if(!this->_fileobject)
        return false;

    return this->_fileobject->hasThumbnail();
}

void QQuickBaseItem::setVersion(const QString &version)
{
    if(this->_version == version)
        return;

    this->_version = version;
    emit versionChanged();
}

void QQuickBaseItem::download()
{
    if(!this->_fileobject)
        return;

    this->_fileobject->download();
}

QQuickItem *QQuickBaseItem::createComponent(const QString &componentcode)
{
    QQmlComponent* component = NULL;
    QByteArray hash = QCryptographicHash::hash(componentcode.toUtf8(), QCryptographicHash::Md5);

    if(!QQuickBaseItem::_components.contains(hash))
    {
        QQmlEngine *engine = qmlEngine(this);

        if(!engine)
        {
            qFatal("Cannot get QML engine instance");
            return NULL;
        }

        component = new QQmlComponent(engine); // TODO: When I need to deallocate these?

        component->setData(QString("import QtQuick %1\n"
                                   "import QtMultimedia 5.0\n"
                                   "import QtGraphicalEffects 1.0\n"
                                   "%2").arg(this->_version, componentcode).toUtf8(), QUrl());

        QQuickBaseItem::_components[hash] = component;
    }
    else
        component = QQuickBaseItem::_components[hash];

    QQmlContext *context = qmlContext(this);

    if(!context)
    {
        qFatal("Cannot get QML context");
        return NULL;
    }

    QQuickItem* item = qobject_cast<QQuickItem*>(component->create(context));

    if(!item)
    {
        qWarning() << component->errorString();
        return NULL;
    }

    item->setParent(this);
    item->setParentItem(this);
    return item;
}

FileObject *QQuickBaseItem::createFileObject(TelegramObject *telegramobject)
{
    if(!telegramobject)
    {
        qWarning("Invalid telegram object");
        return NULL;
    }

    this->_fileobject = FileCache_fileObject(telegramobject);

    if(!this->_fileobject)
        return NULL;

    connect(this->_fileobject, &FileObject::imageSizeChanged, this, &QQuickBaseItem::imageSizeChanged);
    connect(this->_fileobject, &FileObject::downloadedChanged, this, &QQuickBaseItem::downloadedChanged);
    connect(this->_fileobject, &FileObject::downloadingChanged, this, &QQuickBaseItem::downloadingChanged);
    connect(this->_fileobject, &FileObject::hasThumbnailChanged, this, &QQuickBaseItem::hasThumbnailChanged);
    connect(this->_fileobject, &FileObject::thumbnailChanged, [this]() { this->bindToElement(); });
    connect(this->_fileobject, &FileObject::filePathChanged, [this]() { this->bindToElement(); });

    return this->_fileobject;
}

void QQuickBaseItem::createImageElement()
{
    this->_mediaelement = this->createComponent("Image {\n"
                                                     "anchors.fill: parent\n"
                                                     "asynchronous: true\n"
                                                "}");
}

void QQuickBaseItem::createAnimatedElement()
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

    this->_mediaelement = this->createComponent(componentsource);
}

QString QQuickBaseItem::thumbnail() const
{
    if(!this->_fileobject)
        return QString();

    return this->_fileobject->thumbnail();
}

QString QQuickBaseItem::filePath() const
{
    if(!this->_fileobject)
        return QString();

    return this->_fileobject->filePath();
}

void QQuickBaseItem::bindToElement()
{
    if(!this->_fileobject || !this->_mediaelement)
        return;

    QUrl mediaurl;

    if(this->_fileobject->downloaded())
        mediaurl = QUrl::fromLocalFile(this->_fileobject->filePath());
    else if(this->hasThumbnail())
        mediaurl = QUrl::fromLocalFile(this->_fileobject->thumbnail());
    else
        return;

    this->_mediaelement->setProperty("source", mediaurl);
}

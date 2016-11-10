#ifndef QQUICKMEDIAMESSAGEITEM_H
#define QQUICKMEDIAMESSAGEITEM_H

#include "qquickbaseitem.h"

class QQuickMediaMessageItem : public QQuickBaseItem
{
    Q_OBJECT

    Q_PROPERTY(Message* message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(bool isSticker READ isSticker NOTIFY isStickerChanged)
    Q_PROPERTY(bool isAnimated READ isAnimated NOTIFY isAnimatedChanged)
    Q_PROPERTY(bool isVideo READ isVideo NOTIFY isVideoChanged)
    Q_PROPERTY(qreal size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(qreal contentWidth READ contentWidth NOTIFY contentWidthChanged)
    Q_PROPERTY(QString venueTitle READ venueTitle NOTIFY venueTitleChanged)
    Q_PROPERTY(QString venueAddress READ venueAddress NOTIFY venueAddressChanged)
    Q_PROPERTY(QString webPageTitle READ webPageTitle NOTIFY webPageTitleChanged)
    Q_PROPERTY(QString webPageDescription READ webPageDescription NOTIFY webPageDescriptionChanged)
    Q_PROPERTY(QString webPageUrl READ webPageUrl NOTIFY webPageUrlChanged)
    Q_PROPERTY(bool webPageHasPhoto READ webPageHasPhoto NOTIFY webPageHasPhotoChanged)
    Q_PROPERTY(GeoPoint* geoPoint READ geoPoint NOTIFY geoPointChanged)
    Q_PROPERTY(QQmlComponent* imageDelegate READ imageDelegate WRITE setImageDelegate NOTIFY imageDelegateChanged)
    Q_PROPERTY(QQmlComponent* animatedDelegate READ animatedDelegate WRITE setAnimatedDelegate NOTIFY animatedDelegateChanged)
    Q_PROPERTY(QQmlComponent* locationDelegate READ locationDelegate WRITE setLocationDelegate NOTIFY locationDelegateChanged)
    Q_PROPERTY(QQmlComponent* webPageDelegate READ webPageDelegate WRITE setWebPageDelegate NOTIFY webPageDelegateChanged)
    Q_PROPERTY(QQmlComponent* fileDelegate READ fileDelegate WRITE setFileDelegate NOTIFY fileDelegateChanged)

    public:
        QQuickMediaMessageItem(QQuickItem *parent = 0);
        Message* message() const;
        bool isSticker() const;
        bool isAnimated() const;
        bool isVideo() const;
        qreal size() const;
        qreal contentWidth() const;
        QString venueTitle() const;
        QString venueAddress() const;
        QString webPageTitle() const;
        QString webPageDescription() const;
        QString webPageUrl() const;
        bool webPageHasPhoto() const;
        GeoPoint* geoPoint() const;
        void setMessage(Message* message);
        void setSize(qreal size);

    public:
        QQmlComponent* imageDelegate() const;
        void setImageDelegate(QQmlComponent* imagecomponent);
        QQmlComponent* animatedDelegate() const;
        void setAnimatedDelegate(QQmlComponent* animatedcomponent);
        QQmlComponent* locationDelegate() const;
        void setLocationDelegate(QQmlComponent* locationcomponent);
        QQmlComponent* webPageDelegate() const;
        void setWebPageDelegate(QQmlComponent* webpagecomponent);
        QQmlComponent* fileDelegate() const;
        void setFileDelegate(QQmlComponent* filecomponent);

    public slots:
        virtual void download();

    private slots:
        void updateContentWidth(qreal contentwidth);
        void updateContentWidth();
        void scaleToImageSize();
        void scaleToImage();
        void scaleToColumn();
        void scaleToFree();

    private:
        qreal calcAspectRatio(const QSize &imagesize) const;
        bool canDownload() const;
        void createImageElement();
        void createAnimatedElement();
        void createLocationElement();
        void createWebPageElement();
        void createFileElement();
        void initialize();

    protected:
        virtual void componentComplete();

    signals:
        void messageChanged();
        void isStickerChanged();
        void isAnimatedChanged();
        void isVideoChanged();
        void sizeChanged();
        void contentWidthChanged();
        void venueTitleChanged();
        void venueAddressChanged();
        void webPageTitleChanged();
        void webPageDescriptionChanged();
        void webPageUrlChanged();
        void webPageHasPhotoChanged();
        void geoPointChanged();
        void imageDelegateChanged();
        void animatedDelegateChanged();
        void locationDelegateChanged();
        void webPageDelegateChanged();
        void fileDelegateChanged();

    private:
        Message* _message;
        qreal _size;
        qreal _contentwidth;

    private:
        QQmlComponent* _imagecomponent;
        QQmlComponent* _animatedcomponent;
        QQmlComponent* _locationcomponent;
        QQmlComponent* _webpagecomponent;
        QQmlComponent* _filecomponent;
};

#endif // QQUICKMEDIAMESSAGEITEM_H

#ifndef QQUICKMEDIAMESSAGEITEM_H
#define QQUICKMEDIAMESSAGEITEM_H

#include "qquickbaseitem.h"

class QQuickMediaMessageItem : public QQuickBaseItem
{
    Q_OBJECT

    Q_PROPERTY(Message* message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(bool isSticker READ isSticker NOTIFY isStickerChanged)
    Q_PROPERTY(bool isAnimated READ isAnimated NOTIFY isAnimatedChanged)
    Q_PROPERTY(qreal size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(qreal contentWidth READ contentWidth NOTIFY contentWidthChanged)
    Q_PROPERTY(QJSValue locationDelegate READ locationDelegate WRITE setLocationDelegate NOTIFY locationDelegateChanged)

    public:
        QQuickMediaMessageItem(QQuickItem *parent = 0);
        Message* message() const;
        bool isSticker() const;
        bool isAnimated() const;
        qreal size() const;
        qreal contentWidth() const;
        const QJSValue& locationDelegate() const;
        void setMessage(Message* message);
        void setSize(qreal size);
        void setLocationDelegate(const QJSValue& locationdelegate);

    private slots:
        void updateContentWidth(qreal contentwidth);
        void updateContentWidth();
        void scaleToImageSize();
        void scaleToImage();
        void scaleToColumn();

    private:
        qreal calcAspectRatio(const QSize &imagesize) const;
        void callLocationDelegate();
        void createImageElement();
        void createAnimatedElement();
        void createLocationElement();
        void createWebPagePhotoElement();
        void initialize();

    signals:
        void messageChanged();
        void isStickerChanged();
        void isAnimatedChanged();
        void sizeChanged();
        void contentWidthChanged();
        void locationDelegateChanged();

    private:
        Message* _message;
        qreal _size;
        qreal _contentwidth;
        QJSValue _locationdelegate;
};

#endif // QQUICKMEDIAMESSAGEITEM_H

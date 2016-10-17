#ifndef QQUICKMEDIAMESSAGEITEM_H
#define QQUICKMEDIAMESSAGEITEM_H

#include "qquickbaseitem.h"

class QQuickMediaMessageItem : public QQuickBaseItem
{
    Q_OBJECT

    Q_PROPERTY(Message* message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(bool isSticker READ isSticker NOTIFY isStickerChanged)
    Q_PROPERTY(bool isAnimated READ isAnimated NOTIFY isAnimatedChanged)
    Q_PROPERTY(int size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(QJSValue locationDelegate READ locationDelegate WRITE setLocationDelegate NOTIFY locationDelegateChanged)

    public:
        QQuickMediaMessageItem(QQuickItem *parent = 0);
        Message* message() const;
        bool isSticker() const;
        bool isAnimated() const;
        int size() const;
        const QJSValue& locationDelegate() const;
        void setMessage(Message* message);
        void setSize(int size);
        void setLocationDelegate(const QJSValue& locationdelegate);

    private slots:
        void updateMetrics();

    private:
        void callLocationDelegate();
        bool documentIsSticker(Document* document) const;
        bool documentIsAnimated(Document* document) const;
        void createLocationElement();
        void initialize();

    signals:
        void messageChanged();
        void isStickerChanged();
        void isAnimatedChanged();
        void sizeChanged();
        void locationDelegateChanged();

    private:
        Message* _message;
        int _size;
        QJSValue _locationdelegate;
};

#endif // QQUICKMEDIAMESSAGEITEM_H

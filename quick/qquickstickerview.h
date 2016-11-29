#ifndef QQUICKSTICKERVIEW_H
#define QQUICKSTICKERVIEW_H

#include "qquickbaseitem.h"

class QQuickStickerView : public QQuickBaseItem
{
    Q_OBJECT

    Q_PROPERTY(Document* sticker READ sticker WRITE setSticker NOTIFY stickerChanged)
    Q_PROPERTY(QQmlComponent* delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)

    public:
        explicit QQuickStickerView(QQuickItem *parent = 0);
        Document* sticker() const;
        void setSticker(Document* sticker);
        QQmlComponent* delegate() const;
        void setDelegate(QQmlComponent* delegate);

    signals:
        void stickerChanged();
        void delegateChanged();

    protected:
        virtual void componentComplete();

    private:
        Document* _sticker;
        QQmlComponent* _delegate;
};

#endif // QQUICKSTICKERVIEW_H

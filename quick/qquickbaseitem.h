#ifndef QQUICKBASEITEM_H
#define QQUICKBASEITEM_H

#include <QtQuick>
#include <QHash>
#include "../config/cache/filecache.h"

class QQuickBaseItem : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QString version READ version WRITE setVersion NOTIFY versionChanged)
    Q_PROPERTY(QSize imageSize READ imageSize NOTIFY imageSizeChanged)
    Q_PROPERTY(bool downloaded READ downloaded NOTIFY downloadedChanged)
    Q_PROPERTY(bool downloading READ downloading NOTIFY downloadingChanged)
    Q_PROPERTY(bool hasThumbnail READ hasThumbnail NOTIFY hasThumbnailChanged)

    public:
        explicit QQuickBaseItem(QQuickItem *parent = 0);
        QString version() const;
        QSize imageSize() const;
        bool downloaded() const;
        bool downloading() const;
        bool hasThumbnail() const;
        void setVersion(const QString& version);

    public slots:
        void download();

    protected:
        QQuickItem *createComponent(const QString& componentcode);
        FileObject* createFileObject(TelegramObject* telegramobject);
        void createImageElement();
        void createAnimatedElement();
        QString thumbnail() const;
        QString filePath() const;
        void bindToElement();

    signals:
        void versionChanged();
        void imageSizeChanged();
        void downloadedChanged();
        void downloadingChanged();
        void hasThumbnailChanged();

    private:
        FileObject* _fileobject;
        QString _version;

    protected:
        QQuickItem* _mediaelement;

    private:
        static QHash<QByteArray, QQmlComponent*> _components;
};

#endif // QQUICKBASEITEM_H

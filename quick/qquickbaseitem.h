#ifndef QQUICKBASEITEM_H
#define QQUICKBASEITEM_H

#include <QtQuick>
#include <QHash>
#include "../cache/file/filecache.h"
#include "../types/telegramhelper.h"

class QQuickBaseItem : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QString source READ source NOTIFY sourceChanged)
    Q_PROPERTY(QString thumbnail READ thumbnail NOTIFY thumbnailChanged)
    Q_PROPERTY(QString fileSize READ fileSize NOTIFY fileSizeChanged)
    Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool downloaded READ downloaded NOTIFY downloadedChanged)
    Q_PROPERTY(bool downloading READ downloading NOTIFY downloadingChanged)
    Q_PROPERTY(bool uploading READ uploading NOTIFY uploadingChanged)
    Q_PROPERTY(bool hasThumbnail READ hasThumbnail NOTIFY hasThumbnailChanged)

    public:
        explicit QQuickBaseItem(QQuickItem *parent = 0);
        QString source() const;
        QString thumbnail() const;
        QString fileSize() const;
        QString fileName() const;
        QSize imageSize() const;
        qreal progress() const;
        bool downloaded() const;
        bool downloading() const;
        bool uploading() const;
        bool hasThumbnail() const;
        void setVersion(const QString& version);

    public slots:
        virtual void download();

    protected:
        QString escape(const TLString& s);
        void createObject(QQmlComponent* component);
        FileObject* createFileObject(TelegramObject* telegramobject);
        QString filePath() const;

    signals:
        void filePathChanged();
        void fileNameChanged();
        void fileSizeChanged();
        void sourceChanged();
        void thumbnailChanged();
        void imageSizeChanged();
        void downloadedChanged();
        void downloadingChanged();
        void uploadingChanged();
        void hasThumbnailChanged();
        void progressChanged();

    protected:
        FileObject* _fileobject;
        QQuickItem* _mediaelement;
};

#endif // QQUICKBASEITEM_H

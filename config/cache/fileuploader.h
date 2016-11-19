#ifndef FILEUPLOADER_H
#define FILEUPLOADER_H

#include <QObject>
#include <QFileInfo>
#include <QMimeDatabase>
#include "../../types/basic.h"
#include "../../mtproto/dc/dcsessionmanager.h"

class FileUploader : public QObject
{
    Q_OBJECT

    public:
        enum MediaType {
            Document = 0,
            Photo = 1,
        };

    public:
        explicit FileUploader(MediaType mediatype, const QString& storagepath, QObject *parent = 0);
        ~FileUploader();
        MediaType mediaType() const;
        TLLong localFileId() const;
        QString caption() const;
        QString md5hash() const;
        TLInt partsCount() const;
        qreal progress() const;
        bool uploading() const;
        bool isBigFile() const;
        void setCaption(const QString& caption);
        void upload(QString filepath);

    public: // File info
        QString fileName() const;
        QString mimeType() const;
        QSize imageSize() const;

    private:
        bool calculatePartsLength(const QFileInfo* fileinfo);
        void calculatePartsCount(const QFileInfo* fileinfo);
        void scaleImageIfNeeded(QString& filepath);
        void getNextPart(TLBytes &data);

    private slots:
        void uploadPart();
        void onSaveFilePartReplied(MTProtoReply* mtreply);

    signals:
        void progressChanged();
        void uploadingChanged();
        void failed();
        void pending();
        void completed();

    private:
        MediaType _mediatype;
        DCSession* _dcsession;
        QString _storagepath;
        QString _caption;
        QString _filename;
        QString _md5hash;
        QString _mimetype;
        bool _isbigfile;
        bool _deleteoncompleted;
        QFile _file;
        TLLong _localfileid;
        TLLong _partsize;
        TLInt _partscount;
        TLInt _partnum;

    private:
        static QMimeDatabase _mimedb;
};

#endif // FILEUPLOADER_H

#ifndef FILEUPLOADER_H
#define FILEUPLOADER_H

#include <QObject>
#include <QSize>
#include <QFile>
#include <QMimeDatabase>
#include "../../types/basic.h"
#include "../../mtproto/dc/dcsessionmanager.h"

class FileUploader : public QObject
{
    Q_OBJECT

    public:
        enum MediaType {
            MediaDocument = 0,
            MediaPhoto    = 1,
        };

    public:
        explicit FileUploader(const QString& filepath, MediaType mediatype, const QString& storagepath, QObject *parent = 0);
        ~FileUploader();
        TLLong localFileId() const;
        const QString& filePath() const;
        const QSize& imageSize() const;
        qreal progress() const;
        bool uploading() const;
        void setCaption(const QString& caption);
        void upload();

    public: // Telegram Types
        InputMedia* createInputMedia() const;
        Photo* createPhoto() const;
        Document* createDocument() const;

    private:
        InputFile* createInputFile() const;
        InputMedia* createInputMediaPhoto() const;
        InputMedia* createInputMediaDocument() const;
        void scaleImageIfNeeded();
        bool calculatePartsLength();
        void calculatePartsCount();
        void analyzeFile(const QString &filepath);
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
        QString _caption;
        QString _filepath;
        QString _filename;
        QString _md5hash;
        QString _mimetype;
        QSize _imagesize;
        TLLong _partsize;
        TLInt _filesize;

    private:
        int _mediatype;
        DCSession* _dcsession;
        QString _storagepath;
        QFile _file;
        bool _deleteoncompleted;
        bool _isbigfile;
        TLLong _localfileid;
        TLInt _partscount;
        TLInt _partnum;

    private:
        static QMimeDatabase _mimedb;
};

#endif // FILEUPLOADER_H

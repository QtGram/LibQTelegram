#ifndef FILEUPLOADER_H
#define FILEUPLOADER_H

#include <QObject>
#include <QFileInfo>
#include "../../types/basic.h"
#include "../../mtproto/dc/dcsessionmanager.h"

class FileUploader : public QObject
{
    Q_OBJECT

    public:
        explicit FileUploader(QObject *parent = 0);
        ~FileUploader();
        TLLong fileId() const;
        QString caption() const;
        QString fileName() const;
        QString md5hash() const;
        TLInt partsCount() const;
        bool isBigFile() const;
        void setCaption(const QString& caption);
        void upload(const QUrl &filepath);

    private:
        bool calculatePartsLength(const QFileInfo* fileinfo);
        void calculatePartsCount(const QFileInfo* fileinfo);
        void getNextPart(TLBytes &data);

    private slots:
        void uploadPart();
        void onSaveFilePartReplied(MTProtoReply* mtreply);

    signals:
        void failed();
        void pending();
        void completed();

    private:
        DCSession* _dcsession;
        QString _caption;
        QString _filename;
        QString _md5hash;
        bool _isbigfile;
        QFile _file;
        TLLong _fileid;
        TLLong _partsize;
        TLInt _partscount;
        TLInt _partnum;
};

#endif // FILEUPLOADER_H

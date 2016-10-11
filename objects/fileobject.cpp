#include "fileobject.h"
#include "../types/telegramhelper.h"
#include <QDir>

#define BLOCK_SIZE (128 * 1024)

FileObject::FileObject(QObject *parent): QObject(parent)
{
    this->_downloadmode = FileObject::None;
    this->_locthumbnail = NULL;
    this->_locfile = NULL;
    this->_inputfilelocation = NULL;
    this->_dcsession = NULL;
    this->_file = NULL;
}

FileObject::FileObject(const QString &storagepath, QObject *parent): QObject(parent), _storagepath(storagepath)
{
    this->_downloadmode = FileObject::None;
    this->_locthumbnail = NULL;
    this->_locfile = NULL;
    this->_inputfilelocation = NULL;
    this->_dcsession = NULL;
    this->_file = NULL;
}

QString FileObject::thumbnail() const
{
    return this->_thumbnail;
}

QString FileObject::filePath() const
{
    return this->_filepath;
}

void FileObject::setThumbnailLocation(FileLocation *filelocation)
{
    this->_locthumbnail = filelocation;
}

void FileObject::setFileLocation(FileLocation *filelocation)
{
    this->_locfile = filelocation;
}

void FileObject::setFileId(const QString &fileid)
{
    this->_fileid = fileid;
}

void FileObject::setThumbnailId(const QString &thumbnailid)
{
    this->_thumbnailid = thumbnailid;
}

void FileObject::downloadThumbnail()
{
    if(!this->_locthumbnail || (this->_locthumbnail->constructorId() == TLTypes::FileLocationUnavailable))
        return;

    this->_downloadmode = FileObject::DownloadThumbnail;
    this->_inputfilelocation = TelegramHelper::inputFileLocation(this->_locthumbnail);
    this->_dcsession = DC_CreateSession(this->_locthumbnail->dcId());
    this->sendDownloadRequest();
}

bool FileObject::loadCache()
{
    QDir dir(this->_storagepath);

    if(QFile::exists(dir.absoluteFilePath(this->_thumbnailid)))
    {
        this->_thumbnailid = dir.absoluteFilePath(this->_thumbnailid);
        emit thumbnailChanged();
        emit downloadCompleted();
        return true;
    }

    return false;
}

void FileObject::download()
{

}

QString FileObject::extension(const UploadFile *uploadfile)
{
    StorageFileType* storagefile = uploadfile->type();

    if(storagefile->constructorId() == TLTypes::StorageFileGif)
        return ".gif";

    if(storagefile->constructorId() == TLTypes::StorageFileJpeg)
        return ".jpeg";

    if(storagefile->constructorId() == TLTypes::StorageFileMov)
        return ".mov";

    if(storagefile->constructorId() == TLTypes::StorageFileMp3)
        return ".mp3";

    if(storagefile->constructorId() == TLTypes::StorageFileMp4)
        return ".mp4";

    if(storagefile->constructorId() == TLTypes::StorageFilePdf)
        return ".pdf";

    if(storagefile->constructorId() == TLTypes::StorageFilePng)
        return ".png";

    if(storagefile->constructorId() == TLTypes::StorageFileWebp)
        return ".webp";

    if(storagefile->constructorId() == TLTypes::StorageFilePartial)
        return ".temp";

    return QString();
}

void FileObject::sendDownloadRequest()
{
    TLInt offset = (this->_file ? this->_file->size() : 0);
    MTProtoRequest* req = TelegramAPI::uploadGetFile(this->_dcsession, this->_inputfilelocation, offset, BLOCK_SIZE);
    connect(req, &MTProtoRequest::replied, this, &FileObject::onUploadFile);
}

void FileObject::onUploadFile(MTProtoReply *mtreply)
{
    Q_ASSERT(this->_downloadmode != FileObject::None);

    UploadFile uploadfile;
    uploadfile.read(mtreply);

    if(!this->_file)
    {
        QDir dir(this->_storagepath);
        QString filename = (this->_downloadmode == FileObject::DownloadThumbnail) ?
                           this->_thumbnailid : this->_fileid;

        this->_file = new QFile(dir.absoluteFilePath(filename));

        if(!this->_file->open(QFile::WriteOnly))
        {
            qWarning() << "Cannot write file:" << this->_file->fileName();
            this->_file->deleteLater();
            return;
        }

        this->_file->write(uploadfile.bytes());
    }

    if(uploadfile.type()->constructorId() == TLTypes::StorageFilePartial)
    {
        this->sendDownloadRequest();
        return;
    }

    this->_file->close();

    if(this->_downloadmode == FileObject::DownloadThumbnail)
    {
        this->_thumbnail = this->_file->fileName();
        emit thumbnailChanged();
    }
    else
    {
        this->_filepath = this->_file->fileName();
        emit filePathChanged();
    }

    DC_CloseSession(this->_dcsession);

    this->_file = NULL;
    this->_dcsession = NULL;
    this->_downloadmode = FileObject::None;
    emit downloadCompleted();
}

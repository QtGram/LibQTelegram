#include "fileobject.h"
#include "../types/telegramhelper.h"
#include <QDir>

#define BLOCK_SIZE (128 * 1024)

FileObject::FileObject(QObject *parent): QObject(parent)
{
    this->_downloadmode = FileObject::None;
    this->_document = NULL;
    this->_locthumbnail = NULL;
    this->_locfile = NULL;
    this->_inputfilelocation = NULL;
    this->_dcsession = NULL;
    this->_file = NULL;

}

FileObject::FileObject(const QString &storagepath, QObject *parent): QObject(parent), _storagepath(storagepath)
{
    this->_downloadmode = FileObject::None;
    this->_document = NULL;
    this->_locthumbnail = NULL;
    this->_locfile = NULL;
    this->_inputfilelocation = NULL;
    this->_dcsession = NULL;
    this->_file = NULL;

    connect(this, &FileObject::filePathChanged, this, &FileObject::downloadedChanged);
}

bool FileObject::downloaded() const
{
    return !this->_filepath.isEmpty();
}

QSize FileObject::imageSize() const
{
    return this->_imagesize;
}

QString FileObject::thumbnail() const
{
    return this->_thumbnail;
}

QString FileObject::filePath() const
{
    return this->_filepath;
}

void FileObject::setDocument(Document *document)
{
    if(this->_document == document)
        return;

    this->_document = document;

    if(document->thumb())
        this->_locthumbnail = document->thumb()->location();

    DocumentAttribute* attribute =  this->documentAttribute(document, TLTypes::DocumentAttributeImageSize);

    if(attribute)
        this->setImageSize(QSize(attribute->w(), attribute->h()));

    attribute =  this->documentAttribute(document, TLTypes::DocumentAttributeVideo);

    if(attribute)
        this->setImageSize(QSize(attribute->w(), attribute->h()));
}

void FileObject::setImageSize(const QSize &imagesize)
{
    if(this->_imagesize == imagesize)
        return;

    this->_imagesize = imagesize;
    emit imageSizeChanged();
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

    if(this->_document && this->autoDownloadDocument(this->_document))
    {
        this->_downloadmode = FileObject::Download;
        this->_inputfilelocation = TelegramHelper::inputFileLocation(this->_document);
        this->_dcsession = DC_CreateSession(this->_document->dcId());
        this->sendDownloadRequest();
        return;
    }

    this->_downloadmode = FileObject::DownloadThumbnail;

    if(!this->_locthumbnail)
        return;

    this->_inputfilelocation = TelegramHelper::inputFileLocation(this->_locthumbnail);
    this->_dcsession = DC_CreateSession(this->_locthumbnail->dcId());
    this->sendDownloadRequest();
}

bool FileObject::loadCache()
{
    QDir dir(this->_storagepath);

    if(QFile::exists(dir.absoluteFilePath(this->_thumbnailid)))
    {
        this->_thumbnail = dir.absoluteFilePath(this->_thumbnailid);
        emit thumbnailChanged();
        return true;
    }

    if(QFile::exists(dir.absoluteFilePath(this->_fileid)))
    {
        this->_filepath = dir.absoluteFilePath(this->_fileid);
        emit filePathChanged();
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
    }

    this->_file->write(uploadfile.bytes());

    if(uploadfile.bytes().length() == BLOCK_SIZE) // NOTE: We need more data... (needs investigation)
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

bool FileObject::autoDownloadDocument(Document *document)
{
    DocumentAttribute* attribute = this->documentAttribute(document, TLTypes::DocumentAttributeAnimated);

    if(attribute)
        return true;

    attribute = this->documentAttribute(document, TLTypes::DocumentAttributeSticker);

    if(attribute)
        return true;

    return false;
}

DocumentAttribute* FileObject::documentAttribute(Document *document, TLConstructor attributector)
{
    foreach(DocumentAttribute* attribute, document->attributes())
    {
        if(attribute->constructorId() == attributector)
            return attribute;
    }

    return NULL;
}

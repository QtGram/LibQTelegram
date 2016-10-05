#include "mtprotoobject.h"

MTProtoObject::MTProtoObject(QObject *parent) : TelegramObject(parent)
{
    this->_constructorid = MTProtoObject::ctorGzipPacked;
}

void MTProtoObject::read(MTProtoStream *mtstream)
{
    this->_constructorid = mtstream->readTLConstructor();

    Q_ASSERT((this->_constructorid == MTProtoObject::ctorGzipPacked));

    if(this->_constructorid == MTProtoObject::ctorGzipPacked)
        this->_packeddata = mtstream->readTLString();
}

void MTProtoObject::write(MTProtoStream *mtstream)
{
    Q_ASSERT((this->_constructorid == MTProtoObject::ctorGzipPacked));

    mtstream->writeTLConstructor(this->_constructorid);

    if(this->_constructorid == MTProtoObject::ctorGzipPacked)
        mtstream->writeTLString(this->_packeddata);
}

const TLString &MTProtoObject::packedData()
{
    return this->_packeddata;
}

void MTProtoObject::setPackedData(const TLString &packeddata)
{
    if(this->_packeddata == packeddata)
        return;

    this->_packeddata = packeddata;
    emit packedDataChanged();
}

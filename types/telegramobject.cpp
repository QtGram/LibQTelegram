#include "telegramobject.h"

TelegramObject::TelegramObject(QObject *parent) : QObject(parent), _constructorid(0)
{

}

TLConstructor TelegramObject::constructorId() const
{
    return this->_constructorid;
}

void TelegramObject::setConstructorId(TLConstructor constructorid)
{
    if(this->_constructorid == constructorid)
        return;

    this->_constructorid = constructorid;
    emit constructorIdChanged();
}

void TelegramObject::serialize(QByteArray &serializeddata)
{
    MTProtoStream mtstream;

    this->write(&mtstream);
    serializeddata = mtstream.data();
}

void TelegramObject::unserialize(QByteArray &serializeddata)
{
    MTProtoStream mtstream(serializeddata);
    this->read(&mtstream);
}

void TelegramObject::deleteChild(TelegramObject *telegramobject)
{
    if(!telegramobject || (telegramobject->parent() != this))
        return;

    telegramobject->deleteLater();
}

void TelegramObject::compileFlags()
{
    /* NOP */
}

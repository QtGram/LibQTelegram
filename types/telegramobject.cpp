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

void TelegramObject::compileFlags()
{
    /* NOP */
}

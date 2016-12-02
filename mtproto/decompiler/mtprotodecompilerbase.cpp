#include "mtprotodecompilerbase.h"

MTProtoDecompilerBase::MTProtoDecompilerBase(QObject *parent) : QObject(parent)
{

}

bool MTProtoDecompilerBase::isPrintable(const TLString &ba)
{
    for(int i = 0; i < ba.length(); i++)
    {
        QChar ch = ba.at(i);

        if((ch.cell() < 0x20) || (ch.cell() > 0x7E))
            return false;
    }

    return true;
}

TLString MTProtoDecompilerBase::printableString(const TLString &ba)
{
    if(this->isPrintable(ba))
    {
        if(ba.length() > ELIDE_LENGTH)
            return ba.left(ELIDE_LENGTH).simplified().append("...");

        return ba;
    }

    if(ba.length() > ELIDE_LENGTH)
        return ba.left(ELIDE_LENGTH).toUtf8().toHex().append("...");

    return ba.toUtf8().toHex();
}

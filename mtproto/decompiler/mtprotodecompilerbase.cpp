#include "mtprotodecompilerbase.h"

MTProtoDecompilerBase::MTProtoDecompilerBase(QObject *parent) : QObject(parent)
{

}

bool MTProtoDecompilerBase::isPrintable(const QByteArray &ba)
{
    for(int i = 0; i < ba.length(); i++)
    {
        char ch = ba.at(i);

        if((ch < 0x20) || (ch > 0x7E))
            return false;
    }

    return true;
}

QString MTProtoDecompilerBase::printableString(const QByteArray &ba)
{
    if(this->isPrintable(ba))
    {
        if(ba.length() > ELIDE_LENGTH)
            return QString::fromUtf8(ba.left(ELIDE_LENGTH)).simplified().append("...");

        return QString::fromUtf8(ba);
    }

    if(ba.length() > ELIDE_LENGTH)
        return QString(ba.left(ELIDE_LENGTH).toHex()).append("...");

    return QString(ba.toHex());
}

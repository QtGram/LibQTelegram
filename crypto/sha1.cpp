#include "sha1.h"

const int Sha1::BITS_COUNT = 160;
const int Sha1::BYTES_COUNT = Sha1::BITS_COUNT / 8;

Sha1::Sha1()
{

}

QByteArray Sha1::hash(const QByteArray &data)
{
    return QCryptographicHash::hash(data, QCryptographicHash::Sha1);
}

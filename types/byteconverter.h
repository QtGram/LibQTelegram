#ifndef BYTECONVERTER_H
#define BYTECONVERTER_H

#include <QtEndian>
#include <QByteArray>
#include "basic.h"

class ByteConverter
{
    private:
        ByteConverter();

    public:
        template<typename T> static TLBytes serialize(const T& t)
        {
            TLBytes data;
            data.append(reinterpret_cast<const char*>(&t), sizeof(T));
            return data;
        }

        template<typename T> static T integer(const TLBytes& s)
        {
            return qFromBigEndian<T>(reinterpret_cast<const uchar*>(s.constData()));
        }

        template<typename T> static TLBytes bytes(const T& t)
        {
            TLBytes ba;
            ba.fill(0, sizeof(T));

            qToBigEndian(t, reinterpret_cast<uchar*>(ba.data()));
            return ba;
        }
};

#endif // BYTECONVERTER_H

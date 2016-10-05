#ifndef SHA1_H
#define SHA1_H

#include <QtCore>

class Sha1
{
    private:
        Sha1();

    public:
        static QByteArray hash(const QByteArray& data);

    public:
        static const int BITS_COUNT;
        static const int BYTES_COUNT;
};

#endif // SHA1_H"

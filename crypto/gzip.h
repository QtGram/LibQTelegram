#ifndef GZIP_H
#define GZIP_H

#include <zlib.h>
#include <QByteArray>
#include <QDebug>

class GZip
{
    private:
        GZip();

    public:
        static QByteArray uncompress(QByteArray &packeddata);
};

#endif // GZIP_H

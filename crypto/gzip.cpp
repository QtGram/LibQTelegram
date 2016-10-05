#include "gzip.h"

GZip::GZip()
{

}

QByteArray GZip::uncompress(QByteArray &packeddata)
{
    QByteArray unpackeddata;
    unpackeddata.fill(0x00, packeddata.length());

    z_stream stream;
    stream.next_in = reinterpret_cast<Bytef*>(packeddata.data());
    stream.avail_in = packeddata.length();
    stream.total_out = 0;
    stream.zalloc = NULL;
    stream.zfree = NULL;

    if(inflateInit2(&stream, (16 + MAX_WBITS)) != Z_OK)
    {
        qDebug() << Q_FUNC_INFO << "inflateInit2() call failed";
        return QByteArray();
    }

    bool done = false;

    while(!done)
    {
        if(stream.total_out >= static_cast<uLong>(unpackeddata.length()))
            unpackeddata.resize(unpackeddata.length() + (unpackeddata.length() / 2));

        stream.next_out = reinterpret_cast<Bytef*>(unpackeddata.data() + stream.total_out);
        stream.avail_out = unpackeddata.length() - stream.total_out;

        int err = inflate(&stream, Z_SYNC_FLUSH);

        if(err == Z_STREAM_END)
        {
            done = true;
            continue;
        }

        if(err != Z_OK)
        {
            qDebug() << Q_FUNC_INFO << "inflate() call failed";
            break;
        }
    }

    if(inflateEnd(&stream) != Z_OK)
    {
        qDebug() << Q_FUNC_INFO << "inflateEnd() call failed";
        return QByteArray();
    }

    if(done)
        return unpackeddata.left(stream.total_out);

    return QByteArray();
}

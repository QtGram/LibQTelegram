#ifndef MTPROTOSTREAM_H
#define MTPROTOSTREAM_H

#include <QObject>
#include <QIODevice>
#include <QBuffer>
#include <QDebug>
#include "../types/basic.h"
#include "../autogenerated/tltypes.h"

#define READ_TLVECTOR(T) \
    val.reserve(length); \
    for(int i = 0; i < length; i++) \
        val.append(this->read##T()); \

#define WRITE_TLVECTOR(T) \
    for(int i = 0; i < val.length(); i++) \
        this->write##T(val.at(i));

class MTProtoStream : public QObject
{
    Q_OBJECT

    public:
        MTProtoStream(QObject* parent = 0);
        MTProtoStream(const QByteArray &data, QObject* parent = 0);
        virtual ~MTProtoStream();

    public:
        void seek(qint64 off);
        void randPad(int by);
        const QByteArray& data() const;
        TLInt length() const;
        bool atEnd() const;
        TLInt bytesAvailable() const;

    public: // Peek
        TLConstructor peekTLConstructor();

    public: // Read
        TLConstructor readTLConstructor();
        int peekTLVector();
        TLBool readTLBool();
        TLInt readTLInt();
        TLDouble readTLDouble();
        TLInt128 readTLInt128();
        TLInt256 readTLInt256();
        TLLong readTLLong();
        TLBytes readTLBytes();
        TLString readTLString();
        TLBytes readRaw(int len);
        TLBytes readAll();

    public: // Write
        void writeTLConstructor(TLConstructor val);
        void writeTLBool(TLBool val);
        void writeTLInt(TLInt val);
        void writeTLDouble(TLDouble val);
        void writeTLInt128(const TLInt128& val);
        void writeTLInt256(const TLInt256& val);
        void writeTLLong(TLLong val);
        void writeTLBytes(const TLBytes& val);
        void writeTLString(const TLString& val);
        void writeRaw(const QByteArray& data, bool prependlength = true);
        void writeRaw(const MTProtoStream* mtprotostream, bool prependlength = true);

    public:
        template<typename T> void readTLVector(TLVector<T*>& val, bool isbaretype = false);
        template<typename T> void readTLVector(TLVector<T>& val, bool isbaretype = false);
        template<typename T> void writeTLVector(const TLVector<T*>& val, bool isbaretype = false);
        template<typename T> void writeTLVector(const TLVector<T>& val, bool isbaretype = false);

    private:
        void initBuffer();
        int padLength(int len, int by, int maxpad);
        bool align(TLBytes &data, int by, int maxpad);
        template<typename T> bool expect(int size);
        template<typename T> void readTLVectorImpl(TLVector<T>& val, int length);
        template<typename T> void writeTLVectorImpl(const TLVector<T>& val);

    protected:
        QByteArray _data;
        QBuffer _buffer;
};

template<typename T> void MTProtoStream::readTLVector(TLVector<T*> &val, bool isbaretype)
{
    if(!isbaretype)
    {
        TLConstructor ctor = this->readTLConstructor();
        Q_ASSERT(ctor == TLTypes::Vector);
    }

    TLInt length = this->readTLInt();
    Q_ASSERT(length >= 0);

    for(TLInt i = 0; i < length; i++)
    {
        T* t = new T();
        t->read(this);

        val << t;
    }
}


template<typename T> void MTProtoStream::writeTLVector(const TLVector<T*> &val, bool isbaretype)
{
    if(!isbaretype)
        this->writeTLConstructor(TLTypes::Vector);

    this->writeTLInt(val.length());

    for(TLInt i = 0; i < val.length(); i++)
    {
        T* t = val[i];
        t->write(this);
    }
}

template<typename T> void MTProtoStream::readTLVector(TLVector<T> &val, bool isbaretype)
{
    if(!isbaretype)
    {
        TLConstructor ctor = this->readTLConstructor();
        Q_ASSERT(ctor == TLTypes::Vector);
    }

    TLInt length = this->readTLInt();
    Q_ASSERT(length >= 0);

    this->readTLVectorImpl<T>(val, length);
}

template<typename T> void MTProtoStream::writeTLVector(const TLVector<T>& val, bool isbaretype)
{
    if(!isbaretype)
        this->writeTLConstructor(TLTypes::Vector);

    this->writeTLInt(val.length());
    this->writeTLVectorImpl<T>(val);
}

template<typename T> void MTProtoStream::readTLVectorImpl(TLVector<T>&, int) { Q_ASSERT(false); }
template<> inline void MTProtoStream::readTLVectorImpl<TLBool>(TLVector<TLBool>& val, int length) { READ_TLVECTOR(TLBool); }
template<> inline void MTProtoStream::readTLVectorImpl<TLInt>(TLVector<TLInt>& val, int length) { READ_TLVECTOR(TLInt); }
template<> inline void MTProtoStream::readTLVectorImpl<TLLong>(TLVector<TLLong>& val, int length) { READ_TLVECTOR(TLLong); }
template<> inline void MTProtoStream::readTLVectorImpl<TLBytes>(TLVector<TLBytes>& val, int length) { READ_TLVECTOR(TLBytes); }
template<> inline void MTProtoStream::readTLVectorImpl<TLString>(TLVector<TLString>& val, int length) { READ_TLVECTOR(TLString); }

template<typename T> void MTProtoStream::writeTLVectorImpl(const TLVector<T>&) { Q_ASSERT(false); }
template<> inline void MTProtoStream::writeTLVectorImpl<TLBool>(const TLVector<TLBool>& val) { WRITE_TLVECTOR(TLBool); }
template<> inline void MTProtoStream::writeTLVectorImpl<TLInt>(const TLVector<TLInt>& val) { WRITE_TLVECTOR(TLInt); }
template<> inline void MTProtoStream::writeTLVectorImpl<TLLong>(const TLVector<TLLong>& val) { WRITE_TLVECTOR(TLLong); }
template<> inline void MTProtoStream::writeTLVectorImpl<TLBytes>(const TLVector<TLBytes>& val) { WRITE_TLVECTOR(TLBytes); }
template<> inline void MTProtoStream::writeTLVectorImpl<TLString>(const TLVector<TLString>& val) { WRITE_TLVECTOR(TLString); }


template<typename T> bool MTProtoStream::expect(int size)
{
    if(size == sizeof(T))
        return true;

    if(this->_buffer.isReadable())
        qWarning("Expected %ld bytes, readed %d\n", sizeof(T), size);
    else if(this->_buffer.isWritable())
        qWarning("Expected %ld bytes, writed %d\n", sizeof(T), size);
    else
        Q_ASSERT(false);

    return false;
}

#endif // MTPROTOSTREAM_H

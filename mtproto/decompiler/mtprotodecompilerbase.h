#ifndef MTPROTODECOMPILERBASE_H
#define MTPROTODECOMPILERBASE_H

#include <QObject>
#include "../mtprotostream.h"
#include "../../types/basic.h"

#define BIT_FIELD_VALUE(flag, bitno) QString((flag & bitno) ? "true" : "false")

class MTProtoDecompilerBase : public QObject
{
    Q_OBJECT

    public:
        enum Direction { DIRECTION_IN, DIRECTION_OUT };

    public:
        explicit MTProtoDecompilerBase(QObject *parent = 0);
        virtual void decompile(int direction, TLLong messageid, const QByteArray& data) = 0;

    private:
        bool isPrintable(const QByteArray& ba);

    protected:
        QString printableString(const QByteArray& ba);
        template<typename T> void decompileTLVector(QString& result, MTProtoStream& mtstream, bool isbaretype = false);

    private:
        template<typename T> void decompileTLVectorImpl(QString& result, MTProtoStream& mtstream, int length);
};

template<typename T> void MTProtoDecompilerBase::decompileTLVector(QString& result, MTProtoStream& mtstream, bool isbaretype)
{
    if(!isbaretype)
    {
        TLConstructor ctor = mtstream.readTLInt();
        Q_ASSERT(ctor == TLTypes::Vector);
    }

    TLConstructor length = mtstream.readTLInt();
    Q_ASSERT(length >= 0);

    this->decompileTLVectorImpl<T>(result, mtstream, length);
}

template<typename T> void MTProtoDecompilerBase::decompileTLVectorImpl(QString& result, MTProtoStream& mtstream, int length) { Q_ASSERT(false); }

template<> inline void MTProtoDecompilerBase::decompileTLVectorImpl<TLBool>(QString& result, MTProtoStream& mtstream, int length)
{
    result.append("[");

    for(int i = 0; i < length; i++)
    {
        if(i > 0)
            result.append(", ");

        TLBool val = mtstream.readTLBool();
        result.append(QString::number(i) + ":" + (val ? "true" : "false"));
    }

    result.append("]");
}

template<> inline void MTProtoDecompilerBase::decompileTLVectorImpl<TLInt>(QString& result, MTProtoStream& mtstream, int length)
{
    result.append("[");

    for(int i = 0; i < length; i++)
    {
        if(i > 0)
            result.append(", ");

        TLInt val = mtstream.readTLInt();
        result.append(QString::number(i) + ":" + QString::number(val, 16));
    }

    result.append("]");
}

template<> inline void MTProtoDecompilerBase::decompileTLVectorImpl<TLLong>(QString& result, MTProtoStream& mtstream, int length)
{
    result.append("[");

    for(int i = 0; i < length; i++)
    {
        if(i > 0)
            result.append(", ");

        TLLong val = mtstream.readTLLong();
        result.append(QString::number(i) + ":" + QString::number(val, 16));
    }

    result.append("]");
}

template<> inline void MTProtoDecompilerBase::decompileTLVectorImpl<TLBytes>(QString& result, MTProtoStream& mtstream, int length)
{
    result.append("[");

    for(int i = 0; i < length; i++)
    {
        if(i > 0)
            result.append(", ");

        TLBytes val = mtstream.readTLBytes();
        result.append(QString::number(i) + ":" + QString::fromUtf8(val.toHex()));
    }

    result.append("]");
}

#endif // MTPROTODECOMPILERBASE_H

#ifndef BASIC_H
#define BASIC_H

#include <QtGlobal>
#include <QString>

#define IS_FLAG_SET(flags, bitno) ((flags & (1 << bitno)) ? true : false)
#define SET_FLAG_BIT(flags, bitno) flags |= (1 << bitno)
#define UNSET_FLAG_BIT(flags, bitno) flags & ~(1 << bitno)
#define SET_FLAG_BIT_VALUE(flags, bitno, v) flags |= ((v ? 1 : 0) << bitno)

#define RESET_TLTYPE(classtype, type) if(type) type->deleteLater(); \
                                      type = new classtype();

typedef quint32 TLConstructor;
typedef quint8 TLTrue;
typedef bool TLBool;
typedef qint32 TLInt;
typedef qint64 TLLong;
typedef double TLDouble;
typedef QByteArray TLBytes;
typedef QByteArray TLObject;

class TLString: public QByteArray
{
    public:
        TLString(): QByteArray() { }
        TLString(const QByteArray& rhs): QByteArray(rhs) { }
        TLString(const QString& rhs): QByteArray(rhs.toUtf8()) { }
        QString toQString() const { return QString::fromUtf8(*this); }
};

template<typename T> using TLVector = QList<T>;

struct TLInt128
{
    TLLong lo;
    TLLong hi;

    TLInt128(): lo(0), hi(0) { }
    bool operator ==(const TLInt128& rhs) const { return  rhs.hi == hi && rhs.lo == lo; }
};

struct TLInt256
{
    TLInt128 lo;
    TLInt128 hi;

    TLInt256(): lo(), hi() { }
    bool operator ==(const TLInt256& rhs) const { return  rhs.hi == hi && rhs.lo == lo; }
};

#endif // BASIC_H

#ifndef BASIC_H
#define BASIC_H

#include <QtGlobal>
#include <QMetaType>
#include <QString>
#include <inttypes.h>

// Qt < 5.4 compatibility
#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0)
    #define qUtf8Printable(string) QString(string).toUtf8().constData()
#endif

#define IS_FLAG_SET(flags, bitno) ((flags & (1 << bitno)) ? true : false)
#define SET_FLAG_BIT(flags, bitno) flags |= (1 << bitno)
#define UNSET_FLAG_BIT(flags, bitno) flags & ~(1 << bitno)
#define SET_FLAG_BIT_VALUE(flags, bitno, v) flags |= ((v ? 1 : 0) << bitno)

#define ToTLString(s) s.toUtf8()

typedef quint32 TLConstructor;
typedef bool TLTrue;
typedef bool TLBool;
typedef qint32 TLInt;
typedef qint64 TLLong;
typedef double TLDouble;
typedef QByteArray TLBytes;
typedef QByteArray TLObject;

// Internal types
typedef quint64 MessageId;

/*
 * Message ID value:
 * Non Channels: total_number_of_sent_received_messages + 1
 * Channels: Channel message index + 1
 */

/*
 * Group -> Supergroup migration notes:
 *
 * MessageActionChatMigrateTo -> old group side, contains new channel_id
 * MessageActionChannelMigrateFrom -> supergroup side, contains old chat_id, id: 1
 */

class TLString: public QByteArray
{
    public:
        TLString(): QByteArray() { }
        TLString(const QByteArray& rhs): QByteArray(rhs) { }
        TLString(const QString& rhs): QByteArray(rhs.toUtf8()) { }
        QString toString() const { return QString::fromUtf8(*this); }
};


Q_DECLARE_METATYPE(TLString)

template<typename T> using TLVector = QList<T>;

struct TLInt128
{
    TLLong lo;
    TLLong hi;

    TLInt128(): lo(0), hi(0) { }
    bool operator ==(const TLInt128& rhs) const { return  (rhs.hi == hi) && (rhs.lo == lo); }
    bool operator !=(const TLInt128& rhs) const { return  (rhs.hi != hi) || (rhs.lo != lo); }
};

struct TLInt256
{
    TLInt128 lo;
    TLInt128 hi;

    TLInt256(): lo(), hi() { }
    bool operator ==(const TLInt256& rhs) const { return  (rhs.hi == hi) && (rhs.lo == lo); }
    bool operator !=(const TLInt256& rhs) const { return  (rhs.hi != hi) || (rhs.lo != lo); }
};

inline bool is_local_messageid(TLInt messageid) { return messageid < 0; }

inline TLInt local_messageid(TLLong tempid)
{
    TLInt lotempid = static_cast<TLInt>(tempid) & 0xFFFFFFFF;

    if(lotempid < 0)
        return lotempid;

    return -lotempid;
}

#endif // BASIC_H

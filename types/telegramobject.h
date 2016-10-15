#ifndef TELEGRAMOBJECT_H
#define TELEGRAMOBJECT_H

#include <QObject>
#include "types/basic.h"
#include "mtproto/mtprotostream.h"

class TelegramObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(TLConstructor constructorId READ constructorId WRITE setConstructorId NOTIFY constructorIdChanged)

    public:
        explicit TelegramObject(QObject *parent = 0);
        TLConstructor constructorId() const;
        void setConstructorId(TLConstructor constructorid);
        void serialize(QByteArray& serializeddata);
        void unserialize(QByteArray& serializeddata);

    protected:
        template<typename T> void resetTLType(T** t);
        template<typename T> void nullTLType(T** t);
        void deleteChild(TelegramObject* telegramobject);
        virtual void compileFlags();

    public:
        virtual void read(MTProtoStream* mtprotostream) = 0;
        virtual void write(MTProtoStream* mtprotostream) = 0;

    signals:
        void constructorIdChanged();

    protected:
        TLConstructor _constructorid;
};

template<typename T> void TelegramObject::resetTLType(T** t)
{
    if(*t)
        (*t)->deleteLater();

    *t = new T(this);
}

template<typename T> void TelegramObject::nullTLType(T** t)
{
    if(*t)
        (*t)->deleteLater();

    *t = NULL;
}

#endif // TELEGRAMOBJECT_H

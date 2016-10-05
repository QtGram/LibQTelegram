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

    protected:
        virtual void compileFlags();

    public:
        virtual void read(MTProtoStream* mtprotostream) = 0;
        virtual void write(MTProtoStream* mtprotostream) = 0;

    signals:
        void constructorIdChanged();

    protected:
        TLConstructor _constructorid;
};

#endif // TELEGRAMOBJECT_H

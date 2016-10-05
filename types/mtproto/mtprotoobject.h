#ifndef MTPROTOOBJECT_H
#define MTPROTOOBJECT_H

#include "../../types/basic.h"
#include "../../types/telegramobject.h"

class MTProtoObject : public TelegramObject
{
    Q_OBJECT

    Q_PROPERTY(TLString packedData READ packedData WRITE setPackedData NOTIFY packedDataChanged)

    public:
        enum Constructors {
            ctorGzipPacked = 0x3072cfa1,
        };

    public:
        explicit MTProtoObject(QObject *parent = 0);
        virtual void read(MTProtoStream* mtstream);
        virtual void write(MTProtoStream* mtstream);
        const TLString& packedData();
        void setPackedData(const TLString& packeddata);

    signals:
        void packedDataChanged();

    private:
        TLString _packeddata;
};

#endif // MTPROTOOBJECT_H

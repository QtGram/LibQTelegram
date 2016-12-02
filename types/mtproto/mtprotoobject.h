#ifndef MTPROTOOBJECT_H
#define MTPROTOOBJECT_H

#include "../../types/basic.h"
#include "../../types/telegramobject.h"

class MTProtoObject : public TelegramObject
{
    Q_OBJECT

    Q_PROPERTY(TLBytes packedData READ packedData WRITE setPackedData NOTIFY packedDataChanged)

    public:
        enum Constructors {
            ctorGzipPacked = 0x3072cfa1,
        };

    public:
        explicit MTProtoObject(QObject *parent = 0);
        virtual void read(MTProtoStream* mtstream);
        virtual void write(MTProtoStream* mtstream);
        const TLBytes& packedData();
        void setPackedData(const TLBytes& packeddata);

    signals:
        void packedDataChanged();

    private:
        TLBytes _packeddata;
};

#endif // MTPROTOOBJECT_H

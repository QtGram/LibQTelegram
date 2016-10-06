#ifndef MTPROTOREPLY_H
#define MTPROTOREPLY_H

#include "mtprotostream.h"
#include "types/mtproto/mtprotomessage.h"

class MTProtoReply : public MTProtoStream
{
    Q_OBJECT

    public:
        explicit MTProtoReply(const QByteArray& data, int dcid, QObject *parent = 0);
        explicit MTProtoReply(MTProtoMessage* mtmessage, int dcid, QObject *parent = 0);
        explicit MTProtoReply(const QByteArray& data, TLLong messageid, int dcid, QObject *parent = 0);
        virtual ~MTProtoReply();
        bool isError() const;
        int dcid() const;
        TLInt errorCode() const;
        TLLong sessionId() const;
        TLLong messageId() const;
        TLConstructor constructorId() const;
        QByteArray cbody() const;
        QByteArray body();

    private:
        MTProtoReply(int dcid, QObject* parent = 0);
        void readPlainMessage();
        void readEncryptedMessage();

    private:
        int _dcid;
        int _bodystart;
        TLInt128 _messagekey;
        TLLong _authorizationkeyid;
        TLLong _sessionid;
        TLLong _messageid;
        TLInt _messagelength;
        TLInt _errorcode;
        TLConstructor _constructorid;
};

#endif // MTPROTOREPLY_H

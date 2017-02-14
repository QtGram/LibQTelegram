#ifndef MTPROTOREPLY_H
#define MTPROTOREPLY_H

#include "mtprotostream.h"
#include "../types/mtproto/mtprotomessage.h"
#include "../config/telegramconfig.h"

class MTProtoReply : public MTProtoStream
{
    Q_OBJECT

    public:
        explicit MTProtoReply(const QByteArray& data, DCConfig* dcconfig, QObject *parent = 0);
        explicit MTProtoReply(MTProtoMessage* mtmessage, DCConfig* dcconfig, QObject *parent = 0);
        explicit MTProtoReply(const QByteArray& data, TLLong messageid, DCConfig* dcconfig, QObject *parent = 0);
        virtual ~MTProtoReply();
        bool isError() const;
        DCConfig* config() const;
        TLInt errorCode() const;
        TLLong requestId() const;
        TLLong sessionId() const;
        TLLong messageId() const;
        TLConstructor constructorId() const;
        QByteArray cbody() const;
        QByteArray body();
        void seekToBody();

    public:
        void setRequestId(TLLong requestid);

    private:
        MTProtoReply(int dcid, QObject* parent = 0);
        void readPlainMessage();
        void readEncryptedMessage();

    private:
        DCConfig* _dcconfig;
        int _bodystart;
        TLInt128 _messagekey;
        TLLong _requestid;
        TLLong _authorizationkeyid;
        TLLong _sessionid;
        TLLong _messageid;
        TLInt _messagelength;
        TLInt _errorcode;
        TLConstructor _constructorid;
};

#endif // MTPROTOREPLY_H

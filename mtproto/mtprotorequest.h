#ifndef MTPROTOREQUEST_H
#define MTPROTOREQUEST_H

#include <QObject>
#include "mtprotostream.h"
#include "mtprotoreply.h"
#include "../config/telegramconfig.h"

class MTProtoRequest : public QObject
{
    Q_OBJECT

    public:
        explicit MTProtoRequest(DCConfig* dcconfig, QObject *parent = 0);
        ~MTProtoRequest();
        DCConfig* config() const;
        TLLong requestId() const;
        TLLong messageId() const;
        TLLong sessionId() const;
        bool acked() const;
        bool encrypted() const;
        bool needsReply() const;
        const QByteArray& body() const;
        QByteArray build();

    public:
        void setFirst(bool b);
        void setAcked(bool b);
        void setNeedsReply(bool b);
        void setDcConfig(DCConfig* dcconfig);
        void setSessionId(TLLong sessionid);
        void setMessageId(TLLong messageid);
        void setSeqNo(TLInt seqno);
        void setBody(MTProtoStream *body);

    private:
        void initConnection(MTProtoStream& mtstream) const;
        void build(QByteArray& request, const QByteArray& requestbody);
        QByteArray buildEncrypted();
        QByteArray buildPlain();

    protected:
        virtual void timerEvent(QTimerEvent *event);

    signals:
        void timeout(TLLong messageid);
        void replied(MTProtoReply* reply);
        void error();

    private:
        bool _first;
        bool _needsinit;
        bool _needsreply;
        bool _acked;
        TLLong _requestid;
        TLLong _sessionid;
        TLLong _messageid;
        TLInt _seqno;
        DCConfig* _dcconfig;
        MTProtoStream* _body;

    private:
        static TLLong _clientrequestid;
};

#endif // MTPROTOREQUEST_H

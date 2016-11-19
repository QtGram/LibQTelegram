#ifndef MTPROTOREQUEST_H
#define MTPROTOREQUEST_H

#define Try_InitFirst(configid) \
    if(!MTProtoRequest::_firstmap.contains(configid)) \
        MTProtoRequest::_firstmap[configid] = true;

#define IsFirst(configid) MTProtoRequest::_firstmap[configid]
#define UnsetFirst(configid) MTProtoRequest::_firstmap[configid] = false;
#define ResetFirst(configid) MTProtoRequest::_firstmap[configid] = true;

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
        bool acked() const;
        DCConfig* config() const;
        TLLong requestId() const;
        TLLong messageId() const;
        TLLong sessionId() const;
        bool encrypted() const;
        const QByteArray& body() const;
        QByteArray build();

    public:
        static void resetFirst(int dcid);

    public:
        void setAcked(bool b);
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
        bool _needsinit;
        bool _acked;
        TLLong _requestid;
        TLLong _sessionid;
        TLLong _messageid;
        TLInt _seqno;
        DCConfig* _dcconfig;
        MTProtoStream* _body;

    private:
        static QHash<int, bool> _firstmap;
        static TLLong _clientrequestid;
};

#endif // MTPROTOREQUEST_H

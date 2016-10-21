#ifndef MTPROTOREQUEST_H
#define MTPROTOREQUEST_H

#define Try_InitFirst(dcid) \
    if(!MTProtoRequest::_firstmap.contains(dcid)) \
        MTProtoRequest::_firstmap[dcid] = true;

#define IsFirst(dcid) MTProtoRequest::_firstmap[dcid]
#define UnsetFirst(dcid) MTProtoRequest::_firstmap[dcid] = false;
#define ResetFirst(dcid) MTProtoRequest::_firstmap[dcid] = true;

#include <QObject>
#include "mtprotostream.h"
#include "mtprotoreply.h"

class MTProtoRequest : public QObject
{
    Q_OBJECT

    public:
        explicit MTProtoRequest(int dcid, QObject *parent = 0);
        bool acked() const;
        int dcId() const;
        TLLong messageId() const;
        TLLong sessionId() const;
        bool encrypted() const;
        const QByteArray& body() const;
        QByteArray build();

    public:
        static void resetFirst(int dcid);

    public:
        void setAcked(bool b);
        void setDcId(int dcid);
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
        void timerEvent(QTimerEvent *event);

    signals:
        void timeout(TLLong messageid);
        void replied(MTProtoReply* reply);
        void error();

    private:
        bool _acked;
        int _dcid;
        TLLong _sessionid;
        TLLong _messageid;
        TLInt _seqno;
        MTProtoStream* _body;

    private:
        static QHash<int, bool> _firstmap;
};

#endif // MTPROTOREQUEST_H

#ifndef MTPROTOREQUEST_H
#define MTPROTOREQUEST_H

#define TRY_INIT_FIRST(dcid) \
    if(!MTProtoRequest::_firstmap.contains(dcid)) \
        MTProtoRequest::_firstmap[dcid] = true;

#define IS_FIRST(dcid) MTProtoRequest::_firstmap[dcid]
#define UNSET_FIRST(dcid) MTProtoRequest::_firstmap[dcid] = false;

#include <QObject>
#include "mtprotostream.h"
#include "mtprotoreply.h"

class MTProtoRequest : public QObject
{
    Q_OBJECT

    public:
        explicit MTProtoRequest(TLLong& lastmsgid, int dcid, QObject *parent = 0);
        TLConstructor constructorId() const;
        TLLong mainMessageId() const;
        TLLong messageId() const;
        bool encrypted() const;
        const QByteArray& body() const;
        QByteArray build();

    public: // Encrypted setters
        void setSessionId(TLLong sessionid);
        void setSeqNo(TLInt seqno);
        void setBody(MTProtoStream *body);

    private:
        void initConnection(MTProtoStream& mtstream) const;
        void build(QByteArray& request, const QByteArray& requestbody);
        TLLong generateMessageId();
        QByteArray buildEncrypted();
        QByteArray buildPlain();

    signals:
        void replied(MTProtoReply* reply);

    private:
        TLLong& _lastmsgid;
        int _dcid;
        TLConstructor _constructorid;
        TLLong _sessionid;
        TLLong _mainmsgid;
        TLLong _msgid;
        TLInt _seqno;
        MTProtoStream* _body;

    private:
        static QHash<int, bool> _firstmap;
};

#endif // MTPROTOREQUEST_H

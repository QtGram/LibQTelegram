#ifndef DCSESSION_H
#define DCSESSION_H

#include <QTimer>
#include "dc.h"
#include "../../config/telegramconfig.h"
#include "../../types/basic.h"
#include "../mtprotostream.h"
#include "../mtprotorequest.h"

class DCSession : public QObject
{
    Q_OBJECT

    public:
        explicit DCSession(DC* dc, QObject *parent = 0);
        bool ownedDc() const;
        TLLong sessionId() const;
        TLLong* lastMsgId();
        DC* dc() const;
        void setOwnedDC(bool b);
        MTProtoRequest* sendPlain(MTProtoStream *mtstream);
        MTProtoRequest* sendEncrypted(MTProtoStream *mtstream);

    private:
        void generateSessionId();

    private slots:
        void sendAck();
        void queueAck(MTProtoReply *);

    private:
        QTimer* _acktimer;
        TLVector<TLLong> _ackqueue;
        TLLong _sessionid;
        TLLong _lastmsgid;
        DC* _dc;
        bool _owneddc;
};

#endif // DCSESSION_H

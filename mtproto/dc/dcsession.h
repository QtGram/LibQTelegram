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
        ~DCSession();
        bool ownedDc() const;
        TLLong sessionId() const;
        DC* dc() const;
        void setOwnedDC(bool b);
        void sendPlain(MTProtoStream *mtstream);
        MTProtoRequest* sendEncrypted(MTProtoStream *mtstream);

    private:
        void generateSessionId();

    private slots:
        void requestAuthorization();
        void sendAck();
        void queueAck(MTProtoReply *);

    signals:
        void unauthorized(DCSession* dcsession);
        void ready();

    private:
        QTimer* _acktimer;
        TLVector<TLLong> _ackqueue;
        TLLong _sessionid;
        DC* _dc;
};

#endif // DCSESSION_H

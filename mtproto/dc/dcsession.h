#ifndef DCSESSION_H
#define DCSESSION_H

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
        DC* dc() const;
        void setOwnedDC(bool b);
        MTProtoRequest* sendPlain(MTProtoStream *mtstream);
        MTProtoRequest* sendEncrypted(MTProtoStream *mtstream);

    private:
        void generateSessionId();

    private:
        TLLong _sessionid;
        TLLong _lastmsgid;
        DC* _dc;
        bool _owneddc;
};

#endif // DCSESSION_H

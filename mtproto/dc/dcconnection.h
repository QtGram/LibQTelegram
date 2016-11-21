#ifndef DCCONNECTION_H
#define DCCONNECTION_H

#include <QTcpSocket>
#include "../../config/telegramconfig.h"

class DCConnection : public QTcpSocket
{
    Q_OBJECT

    public:
        explicit DCConnection(DCConfig *dcconfig, QObject *parent = 0);
        DCConfig* config() const;

    public slots:
        void connectToDC();

    protected:
        virtual void timerEvent(QTimerEvent *event);

    private:
        void reconnectToDC();

    private slots:
        void onStateChanged(QAbstractSocket::SocketState state);
        void onError(QAbstractSocket::SocketError error);

    signals:
        void failed();

    protected:
        DCConfig* _dcconfig;

    private:
        int _reconnecttimerid;
        int _dctimeouttimerid;
};

#endif // DCCONNECTION_H

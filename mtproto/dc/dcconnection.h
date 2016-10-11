#ifndef DCCONNECTION_H
#define DCCONNECTION_H

#include <QTcpSocket>

class DCConnection : public QTcpSocket
{
    Q_OBJECT

    public:
        explicit DCConnection(const QString& address, quint16 port, int dcid, QObject *parent = 0);
        int id() const;
        void connectToDC();

    protected:
        virtual void timerEvent(QTimerEvent *event);

    private:
        void reconnectToDC();

    private slots:
        void onStateChanged(QAbstractSocket::SocketState state);
        void onError(QAbstractSocket::SocketError error);

    signals:
        void reconnecting();

    private:
        QString _address;
        quint16 _port;
        int _dcid;
        int _reconnecttimerid;
};

#endif // DCCONNECTION_H

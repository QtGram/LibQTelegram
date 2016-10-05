#ifndef DCCONNECTION_H
#define DCCONNECTION_H

#include <QTcpSocket>

class DCConnection : public QTcpSocket
{
    Q_OBJECT

    public:
        enum DcConnectionState { NotConnected, Connecting, Connected, Authorized, Signed };

    public:
        explicit DCConnection(const QString& address, quint16 port, QObject *parent = 0);
        void connectToDC();

    private:
        DcConnectionState _state;
        QString _address;
        quint16 _port;
};

#endif // DCCONNECTION_H

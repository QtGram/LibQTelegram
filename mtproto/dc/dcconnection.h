#ifndef DCCONNECTION_H
#define DCCONNECTION_H

#include <QTcpSocket>

class DCConnection : public QTcpSocket
{
    Q_OBJECT

    public:
        explicit DCConnection(const QString& address, quint16 port, QObject *parent = 0);
        void connectToDC();

    private:
        QString _address;
        quint16 _port;
};

#endif // DCCONNECTION_H

#include "dcconnection.h"

DCConnection::DCConnection(const QString &address, quint16 port, QObject *parent): QTcpSocket(parent), _address(address), _port(port)
{

}

void DCConnection::connectToDC()
{
    this->connectToHost(this->_address, this->_port);
}

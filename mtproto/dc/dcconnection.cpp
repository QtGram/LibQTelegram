#include "dcconnection.h"
#include <QTimerEvent>

DCConnection::DCConnection(const QString &address, quint16 port, int dcid, QObject *parent): QTcpSocket(parent), _address(address), _port(port), _dcid(dcid), _reconnecttimerid(-1)
{
    connect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
}

int DCConnection::id() const
{
    return this->_dcid;
}

void DCConnection::connectToDC()
{
    if(this->state() != DCConnection::UnconnectedState)
        return;

    this->connectToHost(this->_address, this->_port);
}

void DCConnection::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == this->_reconnecttimerid)
    {
        this->connectToDC();
        this->killTimer(this->_reconnecttimerid);
        this->_reconnecttimerid = -1;
    }
}

void DCConnection::reconnectToDC()
{
    this->_reconnecttimerid = this->startTimer(5000);
}

void DCConnection::onStateChanged(SocketState state)
{
    switch(state)
    {
        case QAbstractSocket::UnconnectedState:
            break;

        case QAbstractSocket::HostLookupState:
            qWarning() << "DC" << this->_dcid << "looking up host";
            break;

        case QAbstractSocket::ConnectingState:
            qWarning() << "DC" << this->_dcid << "connecting";
            break;

        case QAbstractSocket::ConnectedState:
            qWarning() << "DC" << this->_dcid << "connected";
            break;

        case QAbstractSocket::BoundState:
            qWarning() << "DC" << this->_dcid << "bound";
            break;

        case QAbstractSocket::ClosingState:
            qWarning() << "DC" << this->_dcid << "closing";
            break;

        case QAbstractSocket::ListeningState:
            qWarning() << "DC" << this->_dcid << "listening";
            break;

        default:
            qWarning() << "DC" << this->_dcid << "unhandled state";
            break;
    }
}

void DCConnection::onError(QAbstractSocket::SocketError error)
{
    switch(error)
    {
        case QAbstractSocket::ConnectionRefusedError:
            qWarning() << "DC" << this->_dcid << "ERROR: Connection refused";
            break;

        case QAbstractSocket::RemoteHostClosedError:
            qWarning() << "DC" << this->_dcid << "ERROR: Remote host closed, reconnecting in 5 seconds...";
            this->reconnectToDC();
            break;

        case QAbstractSocket::HostNotFoundError:
            qWarning() << "DC" << this->_dcid << "ERROR: Host not found";
            break;

        case QAbstractSocket::SocketAccessError:
            qWarning() << "DC" << this->_dcid << "ERROR: Socket access error";
            break;

        case QAbstractSocket::SocketResourceError:
            qWarning() << "DC" << this->_dcid << "ERROR: Socket resource error";
            break;

        case QAbstractSocket::SocketTimeoutError:
            qWarning() << "DC" << this->_dcid << "ERROR: Socket timeout";
            break;

        case QAbstractSocket::DatagramTooLargeError:
            qWarning() << "DC" << this->_dcid << "ERROR: Datagram too large";
            break;

        case QAbstractSocket::NetworkError:
            qWarning() << "DC" << this->_dcid << "ERROR: Network error";
            break;

        case QAbstractSocket::AddressInUseError:
            qWarning() << "DC" << this->_dcid << "ERROR: Address in use";
            break;

        case QAbstractSocket::SocketAddressNotAvailableError:
            qWarning() << "DC" << this->_dcid << "ERROR: Socket address not available";
            break;

        case QAbstractSocket::UnsupportedSocketOperationError:
            qWarning() << "DC" << this->_dcid << "ERROR: Unsupported Socket operation";
            break;

        case QAbstractSocket::ProxyAuthenticationRequiredError:
            qWarning() << "DC" << this->_dcid << "ERROR: Proxy Authentication error";
            break;

        case QAbstractSocket::SslHandshakeFailedError:
            qWarning() << "DC" << this->_dcid << "ERROR: SSL Handshake failed";
            break;

        case QAbstractSocket::UnfinishedSocketOperationError:
            qWarning() << "DC" << this->_dcid << "ERROR: Unfinished Socket operation";
            break;

        case QAbstractSocket::ProxyConnectionRefusedError:
            qWarning() << "DC" << this->_dcid << "ERROR: Proxy connection refused";
            break;

        case QAbstractSocket::ProxyConnectionClosedError:
            qWarning() << "DC" << this->_dcid << "ERROR: Proxy connection closed";
            break;

        case QAbstractSocket::ProxyConnectionTimeoutError:
            qWarning() << "DC" << this->_dcid << "ERROR: Proxy connection timeout";
            break;

        case QAbstractSocket::ProxyNotFoundError:
            qWarning() << "DC" << this->_dcid << "ERROR: Proxy not found timeout";
            break;

        case QAbstractSocket::ProxyProtocolError:
            qWarning() << "DC" << this->_dcid << "ERROR: Proxy protocol error";
            break;

        case QAbstractSocket::OperationError:
            qWarning() << "DC" << this->_dcid << "ERROR: Operation error";
            break;

        case QAbstractSocket::SslInternalError:
            qWarning() << "DC" << this->_dcid << "ERROR: SSL Internal error";
            break;

        case QAbstractSocket::SslInvalidUserDataError:
            qWarning() << "DC" << this->_dcid << "ERROR: SSL Invalid Userdata";
            break;

        case QAbstractSocket::TemporaryError:
            qWarning() << "DC" << this->_dcid << "ERROR: SSL Temporary error";
            break;

        case QAbstractSocket::UnknownSocketError:
            qWarning() << "DC" << this->_dcid << "ERROR: Unknown socket error";
            break;

        default:
            qWarning() << "DC" << this->_dcid << "Unhandled error";
            break;
    }
}

#include "dcconnection.h"
#include "../../types/time.h"
#include <QTimerEvent>

DCConnection::DCConnection(DCConfig* dcconfig, QObject *parent): QTcpSocket(parent), _dcconfig(dcconfig), _reconnecttimerid(0), _dctimeouttimerid(0)
{
    connect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
}

DCConfig *DCConnection::config() const
{
    return this->_dcconfig;
}

void DCConnection::connectToDC()
{
    if(this->state() != DCConnection::UnconnectedState)
        return;

    this->connectToHost(this->_dcconfig->host(), this->_dcconfig->port());
}

void DCConnection::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == this->_reconnecttimerid)
    {
        this->connectToDC();
        this->killTimer(event->timerId());
        this->_reconnecttimerid = 0;
    }
    else if(event->timerId() == this->_dctimeouttimerid)
    {
        this->killTimer(event->timerId());
        this->_dctimeouttimerid = 0;

        if(this->state() == DCConnection::ConnectedState)
            return;

        this->abort();
        emit failed();
    }
}

void DCConnection::reconnectToDC()
{
    this->_reconnecttimerid = this->startTimer(ReconnectionTimeout);
}

void DCConnection::onStateChanged(SocketState state)
{
    switch(state)
    {
        case QAbstractSocket::UnconnectedState:
            qWarning("DC %d unconnected", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::HostLookupState:
            qWarning("DC %d looking up host", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::ConnectingState:
            qWarning("DC %d connecting", this->_dcconfig->dcid());
            this->_dctimeouttimerid = this->startTimer(DCTimeout);
            break;

        case QAbstractSocket::ConnectedState:
            qWarning("DC %d connected", this->_dcconfig->dcid());
            this->setSocketOption(DCConnection::KeepAliveOption, 1);
            break;

        case QAbstractSocket::BoundState:
            qWarning("DC %d bound", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::ClosingState:
            qWarning("DC %d closing", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::ListeningState:
            qWarning("DC %d listening", this->_dcconfig->dcid());
            break;

        default:
            qWarning("DC %d unhandled state", this->_dcconfig->dcid());
            break;
    }
}

void DCConnection::onError(QAbstractSocket::SocketError error)
{
    switch(error)
    {
        case QAbstractSocket::ConnectionRefusedError:
            qWarning("DC %d ERROR: Connection refused", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::RemoteHostClosedError:
            qWarning("DC %d ERROR: Remote host closed, reconnecting in 5 seconds...", this->_dcconfig->dcid());
            this->reconnectToDC();
            break;

        case QAbstractSocket::HostNotFoundError:
            qWarning("DC %d ERROR: Host not found", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::SocketAccessError:
            qWarning("DC %d ERROR: Socket access error", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::SocketResourceError:
            qWarning("DC %d ERROR: Socket resource error", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::SocketTimeoutError:
            qWarning("DC %d ERROR: Socket timeout", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::DatagramTooLargeError:
            qWarning("DC %d ERROR: Datagram too large", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::NetworkError:
            qWarning("DC %d ERROR: Network error", this->_dcconfig->dcid());
            emit failed();
            break;

        case QAbstractSocket::AddressInUseError:
            qWarning("DC %d ERROR: Address in use", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::SocketAddressNotAvailableError:
            qWarning("DC %d ERROR: Socket address not available", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::UnsupportedSocketOperationError:
            qWarning("DC %d ERROR: Unsupported Socket operation", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::ProxyAuthenticationRequiredError:
            qWarning("DC %d ERROR: Proxy Authentication error", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::SslHandshakeFailedError:
            qWarning("DC %d ERROR: SSL Handshake failed", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::UnfinishedSocketOperationError:
            qWarning("DC %d ERROR: Unfinished Socket operation", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::ProxyConnectionRefusedError:
            qWarning("DC %d ERROR: Proxy connection refused", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::ProxyConnectionClosedError:
            qWarning("DC %d ERROR: Proxy connection closed", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::ProxyConnectionTimeoutError:
            qWarning("DC %d ERROR: Proxy connection timeout", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::ProxyNotFoundError:
            qWarning("DC %d ERROR: Proxy not found timeout", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::ProxyProtocolError:
            qWarning("DC %d ERROR: Proxy protocol error", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::OperationError:
            qWarning("DC %d ERROR: Operation error", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::SslInternalError:
            qWarning("DC %d ERROR: SSL Internal error", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::SslInvalidUserDataError:
            qWarning("DC %d ERROR: SSL Invalid Userdata", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::TemporaryError:
            qWarning("DC %d ERROR: SSL Temporary error", this->_dcconfig->dcid());
            break;

        case QAbstractSocket::UnknownSocketError:
            qWarning("DC %d ERROR: Unknown socket error", this->_dcconfig->dcid());
            break;

        default:
            qWarning("DC %d Unhandled error", this->_dcconfig->dcid());
            break;
    }
}

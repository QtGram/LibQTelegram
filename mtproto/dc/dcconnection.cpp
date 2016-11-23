#include "dcconnection.h"
#include "../../types/time.h"
#include <QTimerEvent>

#define ReconnectionBase       2000 // 2 seconds
#define ReconnectionMultiplier 2

DCConnection::DCConnection(DCConfig* dcconfig, bool filedc, QObject *parent): QTcpSocket(parent), _dcconfig(dcconfig), _filedc(filedc), _timreconnect(0), _timdctimeout(0), _reconnectiontimeout(ReconnectionBase)
{
    connect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
}

DCConfig *DCConnection::config() const
{
    return this->_dcconfig;
}

bool DCConnection::fileDc() const
{
    return this->_filedc;
}

void DCConnection::reconnect()
{
    if(this->_timreconnect != 0)
    {
        this->killTimer(this->_timreconnect);
        this->_timreconnect = 0;
    }

    this->_reconnectiontimeout = ReconnectionBase;
    this->connectToDC();
}

void DCConnection::reconnectTimeout()
{
    if(this->_timreconnect != 0)
        return;

    this->warning("Reconnection in %d seconds", this->_reconnectiontimeout / 1000);
    emit timeout(this->_reconnectiontimeout);

    this->_timreconnect = this->startTimer(this->_reconnectiontimeout);
    this->_reconnectiontimeout = this->_reconnectiontimeout * ReconnectionMultiplier;
}

bool DCConnection::connectToDC()
{
    if(this->state() != DCConnection::UnconnectedState)
        return false;

    this->connectToHost(this->_dcconfig->host(), this->_dcconfig->port());
    return true;
}

void DCConnection::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == this->_timreconnect)
    {
        this->killTimer(event->timerId());
        this->_timreconnect = 0;

        if(this->connectToDC())
            this->reconnectTimeout();
    }
    else if(event->timerId() == this->_timdctimeout)
    {
        this->killTimer(event->timerId());
        this->_timdctimeout = 0;

        if(this->state() != DCConnection::ConnectingState)
            return;

        this->abort();
        this->reconnectTimeout();
    }
}

void DCConnection::onStateChanged(SocketState state)
{
    switch(state)
    {
        case QAbstractSocket::UnconnectedState:
            this->log("unconnected");
            break;

        case QAbstractSocket::HostLookupState:
            this->log("looking up host");
            break;

        case QAbstractSocket::ConnectingState:
            this->log("connecting");
            this->_timdctimeout = this->startTimer(DCTimeout);
            break;

        case QAbstractSocket::ConnectedState:
            this->log("connected");
            this->setSocketOption(DCConnection::KeepAliveOption, 1);
            this->_reconnectiontimeout = ReconnectionBase;
            break;

        case QAbstractSocket::BoundState:
            this->log("bound");
            break;

        case QAbstractSocket::ClosingState:
            this->log("closing");
            break;

        case QAbstractSocket::ListeningState:
            this->log("listening");
            break;

        default:
            this->warning("unhandled state (%d)", state);
            break;
    }
}

void DCConnection::onError(QAbstractSocket::SocketError error)
{
    switch(error)
    {
        case QAbstractSocket::ConnectionRefusedError:
            this->warning("ERROR: Connection refused");
            break;

        case QAbstractSocket::RemoteHostClosedError:
            this->warning("ERROR: Remote host closed");
            this->reconnectTimeout();
            break;

        case QAbstractSocket::HostNotFoundError:
            this->warning("ERROR: Host not found");
            break;

        case QAbstractSocket::SocketAccessError:
            this->warning("ERROR: Socket access error");
            break;

        case QAbstractSocket::SocketResourceError:
            this->warning("ERROR: Socket resource error");
            break;

        case QAbstractSocket::SocketTimeoutError:
            this->warning("ERROR: Socket timeout");
            break;

        case QAbstractSocket::DatagramTooLargeError:
            this->warning("ERROR: Datagram too large");
            break;

        case QAbstractSocket::NetworkError:
            this->warning("ERROR: Network error");
            this->reconnectTimeout();
            break;

        case QAbstractSocket::AddressInUseError:
            this->warning("ERROR: Address in use");
            break;

        case QAbstractSocket::SocketAddressNotAvailableError:
            this->warning("ERROR: Socket address not available");
            break;

        case QAbstractSocket::UnsupportedSocketOperationError:
            this->warning("ERROR: Unsupported Socket operation");
            break;

        case QAbstractSocket::ProxyAuthenticationRequiredError:
            this->warning("ERROR: Proxy Authentication error");
            break;

        case QAbstractSocket::SslHandshakeFailedError:
            this->warning("ERROR: SSL Handshake failed");
            break;

        case QAbstractSocket::UnfinishedSocketOperationError:
            this->warning("ERROR: Unfinished Socket operation");
            break;

        case QAbstractSocket::ProxyConnectionRefusedError:
            this->warning("ERROR: Proxy connection refused");
            break;

        case QAbstractSocket::ProxyConnectionClosedError:
            this->warning("ERROR: Proxy connection closed");
            break;

        case QAbstractSocket::ProxyConnectionTimeoutError:
            this->warning("ERROR: Proxy connection timeout");
            break;

        case QAbstractSocket::ProxyNotFoundError:
            this->warning("ERROR: Proxy not found");
            break;

        case QAbstractSocket::ProxyProtocolError:
            this->warning("ERROR: Proxy protocol error");
            break;

        case QAbstractSocket::OperationError:
            this->warning("ERROR: Operation error");
            break;

        case QAbstractSocket::SslInternalError:
            this->warning("ERROR: SSL Internal error");
            break;

        case QAbstractSocket::SslInvalidUserDataError:
            this->warning("ERROR: SSL Invalid Userdata");
            break;

        case QAbstractSocket::TemporaryError:
            this->warning("ERROR: SSL Temporary error");
            break;

        case QAbstractSocket::UnknownSocketError:
            this->warning("ERROR: Unknown socket error");
            break;

        default:
            this->warning("ERROR: Unhandled error (%d)", error);
            break;
    }
}

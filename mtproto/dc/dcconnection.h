#ifndef DCCONNECTION_H
#define DCCONNECTION_H

#include <QTcpSocket>
#include "../../config/telegramconfig.h"

#define DCPrintable(dcconfig, filedc) qUtf8Printable(QString("DC %1%2 ").arg(dcconfig.dcid(), (filedc ? " (file transfer)" : "")))

class DCConnection : public QTcpSocket
{
    Q_OBJECT

    public:
        explicit DCConnection(DCConfig *dcconfig, bool filedc, QObject *parent = 0);
        DCConfig* config() const;
        bool fileDc() const;
        void reconnect();

    public: // Loggings
        template<typename ...ARGS> void log(const QString& msg, ARGS&&... args) const;
        template<typename ...ARGS> void warning(const QString& msg, ARGS&&... args) const;
        template<typename ...ARGS> void fatal(const QString& msg, ARGS&&... args) const;

    public slots:
        void connectToDC();

    protected:
        void reconnectTimeout();
        virtual void timerEvent(QTimerEvent *event);

    private slots:
        void onStateChanged(QAbstractSocket::SocketState state);
        void onError(QAbstractSocket::SocketError error);

    signals:
        void timeout(int seconds);

    protected:
        DCConfig* _dcconfig;
        bool _filedc;

    private:
        int _timreconnect;
        int _timdctimeout;
        int _reconnectiontimeout;
};

template<typename ...ARGS> void DCConnection::log(const QString &msg, ARGS&&... args) const
{
    QString s = "DC %d";

    if(this->_filedc)
        s += " (file transfer)";

    s += " " + msg;
    qDebug(qUtf8Printable(s), this->_dcconfig->dcid(), std::forward<ARGS>(args)...);
}

template<typename ...ARGS> void DCConnection::warning(const QString &msg, ARGS&&... args) const
{
    QString s = "DC %d";

    if(this->_filedc)
        s += " (file transfer)";

    s += " " + msg;
    qWarning(qUtf8Printable(s), this->_dcconfig->dcid(), std::forward<ARGS>(args)...);
}

template<typename ...ARGS> void DCConnection::fatal(const QString &msg, ARGS&&... args) const
{
    QString s = "DC %d";

    if(this->_filedc)
        s += " (file transfer)";

    s += " " + msg;
    qFatal(qUtf8Printable(s), this->_dcconfig->dcid(), std::forward<ARGS>(args)...);
}

#endif // DCCONNECTION_H

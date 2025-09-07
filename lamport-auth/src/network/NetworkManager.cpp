#include "NetworkManager.h"
#include <QDebug>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , m_server(nullptr)
    , m_socket(nullptr)
{
}

NetworkManager::~NetworkManager()
{
    stopServer();
    closeSocket();
}

bool NetworkManager::startServer(const QString &localIp, quint16 localPort, QString &err)
{
    err.clear();
    if (m_server) {
        err = "Server already running";
        return false;
    }

    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &NetworkManager::onNewConnection);

    QHostAddress addr(localIp);
    if (!m_server->listen(addr, localPort)) {
        err = QString("Failed to listen on %1:%2 : %3").arg(localIp).arg(localPort).arg(m_server->errorString());
        m_server->deleteLater();
        m_server = nullptr;
        return false;
    }
    return true;
}

void NetworkManager::stopServer()
{
    if (m_server) {
        m_server->close();
        m_server->deleteLater();
        m_server = nullptr;
    }
    closeSocket();
}

void NetworkManager::connectToPeer(const QString &peerIp, quint16 peerPort, QString &err)
{
    err.clear();
    if (!m_socket) {
        m_socket = new QTcpSocket(this);
        connect(m_socket, &QTcpSocket::readyRead, this, &NetworkManager::onSocketReadyRead);
        connect(m_socket, &QTcpSocket::disconnected, this, &NetworkManager::onSocketDisconnected);
        connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
                this, &NetworkManager::onSocketErrorOccurred);
    }

    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        err = "Already connected";
        return;
    }

    m_inBuffer.clear();
    m_socket->connectToHost(peerIp, peerPort);
    // we don't block waiting for connected here; listen to connected signal if user needs it
    connect(m_socket, &QTcpSocket::connected, [this]() {
        emit connected();
    });
}

void NetworkManager::disconnectFromPeer()
{
    if (m_socket) {
        m_socket->disconnectFromHost();
    }
}

bool NetworkManager::sendLine(const QString &line, QString &err)
{
    err.clear();
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
        err = "Not connected";
        return false;
    }

    QByteArray ba = line.toUtf8();
    ba.append('\n');
    qint64 w = m_socket->write(ba);
    if (w == -1) {
        err = m_socket->errorString();
        return false;
    }
    bool flushed = m_socket->flush();
    Q_UNUSED(flushed);
    return true;
}

bool NetworkManager::isConnected() const
{
    return m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

void NetworkManager::onNewConnection()
{
    // Accept the first incoming connection only. If one exists, we close it and accept new.
    if (!m_server) return;

    QTcpSocket *clientSock = m_server->nextPendingConnection();
    if (!clientSock) return;

    if (m_socket) {
        // Close existing connection
        m_socket->disconnectFromHost();
        m_socket->deleteLater();
        m_socket = nullptr;
    }

    m_socket = clientSock;
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkManager::onSocketReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkManager::onSocketDisconnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
            this, &NetworkManager::onSocketErrorOccurred);

    m_inBuffer.clear();
    emit connected();
}

void NetworkManager::onSocketReadyRead()
{
    if (!m_socket) return;

    QByteArray chunk = m_socket->readAll();
    if (chunk.isEmpty()) return;

    m_inBuffer.append(chunk);

    // Process full lines separated by '\n'
    while (true) {
        int idx = m_inBuffer.indexOf('\n');
        if (idx == -1) break;
        QByteArray line = m_inBuffer.left(idx);
        m_inBuffer.remove(0, idx + 1);
        QString s = QString::fromUtf8(line).trimmed();
        if (!s.isEmpty()) {
            emit messageReceived(s);
        }
    }
}

void NetworkManager::onSocketDisconnected()
{
    emit disconnected();
    closeSocket();
}

void NetworkManager::onSocketErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    if (m_socket) {
        emit errorOccurred(m_socket->errorString());
    } else if (m_server) {
        emit errorOccurred(m_server->errorString());
    }
}

void NetworkManager::closeSocket()
{
    if (m_socket) {
        m_socket->close();
        m_socket->deleteLater();
        m_socket = nullptr;
    }
}

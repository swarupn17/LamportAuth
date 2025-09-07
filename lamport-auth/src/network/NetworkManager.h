#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QString>

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    // Start listening as a server on localIp:localPort
    bool startServer(const QString &localIp, quint16 localPort, QString &err);

    // Stop server / disconnect sockets
    void stopServer();

    // Connect to peer (client mode)
    void connectToPeer(const QString &peerIp, quint16 peerPort, QString &err);

    // Disconnect (client socket)
    void disconnectFromPeer();

    // Send a line (no newline required; newline appended automatically)
    bool sendLine(const QString &line, QString &err);

    // Is connected to a peer?
    bool isConnected() const;

signals:
    void connected();               // emitted when a peer connection is established
    void disconnected();            // emitted when connection is lost
    void messageReceived(const QString &line); // emitted when a full line (without newline) is received
    void errorOccurred(const QString &err);

private slots:
    void onNewConnection();
    void onSocketReadyRead();
    void onSocketDisconnected();
    void onSocketErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    QTcpServer *m_server = nullptr;
    QTcpSocket *m_socket = nullptr; // single peer connection (assignment uses single connection)
    QByteArray m_inBuffer;         // buffer for incoming bytes

    void closeSocket();
};

#endif // NETWORKMANAGER_H

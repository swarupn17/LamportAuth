#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QString>
#include <QTimer>
#include "../network/NetworkManager.h"
#include "../util/ConfigManager.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);
    ~Controller();

    // Load configuration (path to config_alice.json or config_bob.json)
    bool loadConfig(const QString &configPath, QString &err);

    // Expose role getter
    QString role() const;

    // Attach an existing NetworkManager (or create one externally and pass pointer)
    void setNetworkManager(NetworkManager *nm);

    // Start / stop protocol (Start begins challenge-response loop for Alice; Bob remains reactive)
    Q_SLOT void start();
    Q_SLOT void stop();

    // Convenience: start listening as server
    bool startServer(QString &err);

    // Convenience: connect to peer
    void connectToPeer(QString &err);

signals:
    void logMessage(const QString &line);   // UI can append/display this
    void statusChanged(const QString &status);
    void roundChanged(int current, int total); // new: emit when round updates
    void finished();                        // protocol finished or stopped

private slots:
    // Network callbacks
    void onConnected();
    void onDisconnected();
    void onError(const QString &err);
    void onMessage(const QString &line);

    // internal helpers
    void sendChallenge(int c);
    void onChallengeTimeout();

private:
    NetworkManager *m_net = nullptr;
    ConfigManager m_cfg;

    QString m_role;         // "Alice" or "Bob"
    QString m_aliceHn;      // loaded stored hash for Alice (updated after each successful round)
    QString m_bobH0;        // Bob's seed (h0)
    int m_n = 100;
    int m_sleepMs = 1000;

    // Alice state
    int m_currentC = 1;
    bool m_running = false;
    bool m_awaitingResponse = false;

    // Timeout timer if you want to handle missing response (optional)
    QTimer m_responseTimer;
    int m_responseTimeoutMs = 5000; // 5 seconds default

    // Helper logging
    void log(const QString &s);
};

#endif // CONTROLLER_H

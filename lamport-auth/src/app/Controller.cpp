#include "Controller.h"
#include "../crypto/LamportChain.h"
#include <QDebug>
#include <QFileInfo>
#include <QThread>
#include <QDateTime>

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    // response timeout handling
    m_responseTimer.setSingleShot(true);
    connect(&m_responseTimer, &QTimer::timeout, this, &Controller::onChallengeTimeout);
}

Controller::~Controller()
{
    stop();
}

bool Controller::loadConfig(const QString &configPath, QString &err)
{
    if (!m_cfg.load(configPath)) {
        err = m_cfg.lastError();
        return false;
    }

    m_role = m_cfg.role();
    m_n = m_cfg.n();
    m_sleepMs = m_cfg.sleepMs();

    // load role-specific files
    if (m_role.compare("Alice", Qt::CaseInsensitive) == 0) {
        QString hn = m_cfg.readAliceHn();
        if (hn.isEmpty()) {
            err = QString("Failed to read alice hn file: %1").arg(m_cfg.aliceHnFile());
            return false;
        }
        m_aliceHn = hn.trimmed();
        log(QString("Loaded Alice hn: %1").arg(m_aliceHn.left(16) + "..."));
    } else if (m_role.compare("Bob", Qt::CaseInsensitive) == 0) {
        QString h0 = m_cfg.readBobH0();
        if (h0.isEmpty()) {
            err = QString("Failed to read bob h0 file: %1").arg(m_cfg.bobH0File());
            return false;
        }
        m_bobH0 = h0.trimmed();
        log(QString("Loaded Bob h0: %1").arg(m_bobH0.left(16) + "..."));
    } else {
        err = "Invalid role in config (must be 'Alice' or 'Bob').";
        return false;
    }

    // set default response timeout relative to sleep_ms (optional)
    m_responseTimeoutMs = qMax(3000, m_sleepMs * 3);

    emit statusChanged("Config loaded");
    // emit initial round state
    emit roundChanged(0, m_n - 1);
    return true;
}

QString Controller::role() const
{
    return m_role;
}

void Controller::setNetworkManager(NetworkManager *nm)
{
    if (m_net) {
        // disconnect old signals
        disconnect(m_net, nullptr, this, nullptr);
    }
    m_net = nm;
    if (!m_net) return;

    connect(m_net, &NetworkManager::messageReceived, this, &Controller::onMessage);
    connect(m_net, &NetworkManager::connected, this, &Controller::onConnected);
    connect(m_net, &NetworkManager::disconnected, this, &Controller::onDisconnected);
    connect(m_net, &NetworkManager::errorOccurred, this, &Controller::onError);
}

bool Controller::startServer(QString &err)
{
    if (!m_net) {
        err = "NetworkManager not set";
        return false;
    }
    QString localIp = m_cfg.localIp();
    int localPort = m_cfg.localPort();
    if (!m_net->startServer(localIp, (quint16)localPort, err)) {
        return false;
    }
    emit statusChanged("Listening");
    log(QString("Server listening on %1:%2").arg(localIp).arg(localPort));
    return true;
}

void Controller::connectToPeer(QString &err)
{
    if (!m_net) {
        err = "NetworkManager not set";
        return;
    }
    QString peerIp = m_cfg.peerIp();
    int peerPort = m_cfg.peerPort();
    m_net->connectToPeer(peerIp, (quint16)peerPort, err);
    if (err.isEmpty()) {
        emit statusChanged("Connecting");
        log(QString("Connecting to %1:%2").arg(peerIp).arg(peerPort));
    } else {
        emit statusChanged("Connect failed");
        log(QString("Connect failed: %1").arg(err));
    }
}

void Controller::start()
{
    if (!m_net) {
        log("NetworkManager not set. Cannot start.");
        return;
    }
    if (!m_net->isConnected()) {
        log("Not connected to peer. Start after connection established.");
        return;
    }

    if (m_running) {
        log("Already running");
        return;
    }

    m_running = true;
    m_currentC = 1;
    m_awaitingResponse = false;

    if (m_role.compare("Alice", Qt::CaseInsensitive) == 0) {
        log("Alice starting challenge-response");
        // ensure alice has hn loaded
        if (m_aliceHn.isEmpty()) {
            log("Alice hn not loaded. Cannot start.");
            m_running = false;
            return;
        }
        // emit initial round
        emit roundChanged(m_currentC, m_n - 1);
        // send first challenge
        sendChallenge(m_currentC);
    } else {
        log("Bob ready and waiting for challenges");
        // Bob is passive; waits on onMessage(CHALLENGE:...)
    }

    emit statusChanged("Running");
}

void Controller::stop()
{
    if (!m_running) {
        log("Not running");
    }
    m_running = false;
    m_awaitingResponse = false;
    if (m_responseTimer.isActive()) m_responseTimer.stop();
    emit statusChanged("Stopped");
    emit finished();
}

void Controller::onConnected()
{
    log("Network connected");
    emit statusChanged("Connected");
}

void Controller::onDisconnected()
{
    log("Network disconnected");
    emit statusChanged("Disconnected");
    // stop protocol if running
    if (m_running) stop();
}

void Controller::onError(const QString &err)
{
    log(QString("Network error: %1").arg(err));
    emit statusChanged("Error");
    if (m_running) stop();
}

void Controller::onMessage(const QString &line)
{
    if (!m_running && m_role.compare("Bob", Qt::CaseInsensitive) != 0) {
        // If not running and not Bob, ignore messages (Bob always listens)
        log(QString("Message received while not running: %1").arg(line));
        return;
    }

    if (line.startsWith("CHALLENGE:", Qt::CaseInsensitive)) {
        // Bob handles challenges
        if (m_role.compare("Bob", Qt::CaseInsensitive) != 0) {
            log(QString("Unexpected CHALLENGE on non-Bob node: %1").arg(line));
            return;
        }

        QString payload = line.mid(QString("CHALLENGE:").length()).trimmed();
        bool ok = false;
        int c = payload.toInt(&ok);
        if (!ok) {
            log(QString("Invalid challenge payload: %1").arg(payload));
            return;
        }
        log(QString("Received CHALLENGE: %1").arg(c));

        // Validate c
        if (c <= 0 || c >= m_n) {
            log(QString("Challenge out of range: %1").arg(c));
            // optionally send ERROR
            QString err;
            m_net->sendLine(QString("ERROR:Challenge out of range"), err);
            return;
        }

        // Compute r = H^(n-c)(h0)
        int times = m_n - c;
        QString r = QString::fromStdString(LamportChain::HashRepeat(m_bobH0.toStdString(), times));
        QString msg = QString("RESPONSE:%1").arg(r);
        QString err;
        if (m_net->sendLine(msg, err)) {
            log(QString("Sent RESPONSE for challenge %1").arg(c));
        } else {
            log(QString("Failed to send response: %1").arg(err));
        }
        return;
    }

    if (line.startsWith("RESPONSE:", Qt::CaseInsensitive)) {
        // Alice handles response
        if (m_role.compare("Alice", Qt::CaseInsensitive) != 0) {
            log(QString("Unexpected RESPONSE on non-Alice node: %1").arg(line));
            return;
        }

        QString r = line.mid(QString("RESPONSE:").length()).trimmed();
        log(QString("Received RESPONSE: %1...").arg(r.left(16)));

        // verify: HashOnce(r) == storedHash
        bool ok = LamportChain::VerifyResponse(r.toStdString(), m_aliceHn.toStdString());
        if (ok) {
            // update storedHash = r
            m_aliceHn = r;
            log(QString("Verify OK for challenge %1; updated storedHash").arg(m_currentC));
            // notify round increase
            emit roundChanged(m_currentC, m_n - 1);
            // stop response timer
            if (m_responseTimer.isActive()) m_responseTimer.stop();
            m_awaitingResponse = false;

            // increment c and either send next challenge or finish
            m_currentC++;
            if (m_currentC <= m_n - 1) {
                // schedule next challenge after sleep_ms
                QTimer::singleShot(m_sleepMs, [this]() {
                    if (!m_running) return;
                    emit roundChanged(m_currentC, m_n - 1); // next round about to start
                    sendChallenge(m_currentC);
                });
            } else {
                log("All rounds completed (Alice).");
                m_running = false;
                emit finished();
                emit statusChanged("Finished");
            }
        } else {
            log(QString("Verify FAILED for challenge %1. Stopping.").arg(m_currentC));
            m_running = false;
            emit finished();
            emit statusChanged("Verification failed");
        }
        return;
    }

    // control messages
    if (line.compare("STOP", Qt::CaseInsensitive) == 0) {
        log("Received STOP");
        stop();
        return;
    }
    if (line.startsWith("ERROR:", Qt::CaseInsensitive)) {
        QString e = line.mid(6).trimmed();
        log(QString("Peer reported error: %1").arg(e));
        return;
    }

    // unknown
    log(QString("Unknown message: %1").arg(line));
}

void Controller::sendChallenge(int c)
{
    if (!m_net || !m_net->isConnected()) {
        log("Cannot send challenge: not connected");
        m_running = false;
        emit statusChanged("Not connected");
        return;
    }

    QString msg = QString("CHALLENGE:%1").arg(c);
    QString err;
    if (m_net->sendLine(msg, err)) {
        log(QString("Sent CHALLENGE: %1").arg(c));
        m_awaitingResponse = true;
        // start response timeout
        m_responseTimer.start(m_responseTimeoutMs);
    } else {
        log(QString("Failed to send challenge: %1").arg(err));
        m_running = false;
        emit statusChanged("Send failed");
    }
}

void Controller::onChallengeTimeout()
{
    if (!m_awaitingResponse) return;
    log(QString("Response timeout for challenge %1").arg(m_currentC));
    m_running = false;
    m_awaitingResponse = false;
    emit finished();
    emit statusChanged("Timeout");
}

void Controller::log(const QString &s)
{
    QString out = QString("[%1] %2").arg(QDateTime::currentDateTime().toString(Qt::ISODate)).arg(s);
    emit logMessage(out);
    qDebug() << out;
}

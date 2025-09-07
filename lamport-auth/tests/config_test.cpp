#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    const QString path = (argc > 1) ? argv[1] : "config_alice.json";
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        qCritical() << "FAILED: cannot open" << path;
        return 2;
    }

    const QByteArray data = f.readAll();
    const QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qCritical() << "FAILED: invalid JSON in" << path;
        return 3;
    }

    QJsonObject obj = doc.object();
    qDebug() << "Loaded JSON keys:" << obj.keys();

    auto getString = [&](QStringList keys, QString def = "") {
        for (const QString& k : keys) {
            if (obj.contains(k))
                return obj.value(k).toVariant().toString();
        }
        return def;
    };
    auto getInt = [&](QStringList keys, int def = 0) {
        for (const QString& k : keys) {
            if (obj.contains(k))
                return obj.value(k).toInt();
        }
        return def;
    };

    QString localIp = getString({ "local_ip" });
    QString peerIp  = getString({ "peer_ip", "remote_ip" });
    int localPort   = getInt({ "local_port" });
    int peerPort    = getInt({ "peer_port", "port" });
    int sleepMs     = getInt({ "sleep_ms", "sleep" });
    int n           = getInt({ "n" });
    QString h0File  = getString({ "bob_h0_file", "h0_file" });
    QString hnFile  = getString({ "alice_hn_file", "hn_file" });

    qDebug().noquote() << "local_ip:"   << localIp;
    qDebug().noquote() << "peer_ip:"    << peerIp;
    qDebug().noquote() << "local_port:" << localPort;
    qDebug().noquote() << "peer_port:"  << peerPort;
    qDebug().noquote() << "sleep_ms:"   << sleepMs;
    qDebug().noquote() << "n:"          << n;
    qDebug().noquote() << "h0_file:"    << h0File;
    qDebug().noquote() << "hn_file:"    << hnFile;

    return 0;
}

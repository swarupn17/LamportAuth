#include "ConfigManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDir>
#include <QTextStream>

ConfigManager::ConfigManager() {}
ConfigManager::~ConfigManager() {}

bool ConfigManager::load(const QString &filePath)
{
    m_lastError.clear();
    QFile f(filePath);
    if (!f.exists()) {
        m_lastError = QString("Config file does not exist: %1").arg(filePath);
        return false;
    }
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = QString("Cannot open config file: %1").arg(filePath);
        return false;
    }

    QByteArray raw = f.readAll();
    f.close();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(raw, &err);
    if (err.error != QJsonParseError::NoError) {
        m_lastError = QString("JSON parse error: %1 at offset %2").arg(err.errorString()).arg(err.offset);
        return false;
    }
    if (!doc.isObject()) {
        m_lastError = "Config JSON root is not an object";
        return false;
    }

    m_obj = doc.object();
    m_fileDir = QFileInfo(filePath).absolutePath();
    return true;
}

QString ConfigManager::role() const
{
    return m_obj.value("role").toString(QString());
}

QString ConfigManager::localIp() const
{
    return m_obj.value("local_ip").toString("127.0.0.1");
}

int ConfigManager::localPort() const
{
    return m_obj.value("local_port").toInt(0);
}

QString ConfigManager::peerIp() const
{
    return m_obj.value("peer_ip").toString("127.0.0.1");
}

int ConfigManager::peerPort() const
{
    return m_obj.value("peer_port").toInt(0);
}

int ConfigManager::sleepMs() const
{
    return m_obj.value("sleep_ms").toInt(1000);
}

int ConfigManager::n() const
{
    return m_obj.value("n").toInt(100);
}

QString ConfigManager::aliceHnFile() const
{
    QString v = m_obj.value("alice_hn_file").toString();
    if (v.isEmpty()) return QString();
    // Resolve relative to config directory
    QFileInfo fi(m_fileDir, v);
    return fi.absoluteFilePath();
}

QString ConfigManager::bobH0File() const
{
    QString v = m_obj.value("bob_h0_file").toString();
    if (v.isEmpty()) return QString();
    QFileInfo fi(m_fileDir, v);
    return fi.absoluteFilePath();
}

QString ConfigManager::readAliceHn() const
{
    QString path = aliceHnFile();
    if (path.isEmpty()) return QString();
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        const_cast<ConfigManager*>(this)->m_lastError = QString("Cannot open alice_hn_file: %1").arg(path);
        return QString();
    }
    QTextStream in(&f);
    QString line = in.readLine().trimmed();
    f.close();
    return line;
}

QString ConfigManager::readBobH0() const
{
    QString path = bobH0File();
    if (path.isEmpty()) return QString();
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        const_cast<ConfigManager*>(this)->m_lastError = QString("Cannot open bob_h0_file: %1").arg(path);
        return QString();
    }
    QTextStream in(&f);
    QString line = in.readLine().trimmed();
    f.close();
    return line;
}

QString ConfigManager::lastError() const
{
    return m_lastError;
}

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>
#include <QJsonObject>

class ConfigManager
{
public:
    ConfigManager();
    ~ConfigManager();

    // Load config JSON from filePath. Returns true on success; on failure, lastError() contains message.
    bool load(const QString &filePath);

    // Accessors
    QString role() const;
    QString localIp() const;
    int localPort() const;
    QString peerIp() const;
    int peerPort() const;
    int sleepMs() const;
    int n() const;
    QString aliceHnFile() const;
    QString bobH0File() const;

    // Read the stored hash file contents (returns empty string + sets lastError() on failure)
    QString readAliceHn() const;
    QString readBobH0() const;

    QString lastError() const;

private:
    QJsonObject m_obj;
    QString m_lastError;
    QString m_fileDir; // directory of config file (to resolve relative paths)
};

#endif // CONFIGMANAGER_H

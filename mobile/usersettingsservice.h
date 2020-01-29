#ifndef USERSETTINGSSERVICE_H
#define USERSETTINGSSERVICE_H

#include <QObject>
#include <QVariant>
#include <QFile>
#include <QJsonObject>

class Device;

class UserSettingsService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant devices READ devices NOTIFY devicesChanged)

public:
    explicit UserSettingsService(QObject *parent = nullptr);
    ~UserSettingsService();
    QVariant devices() const; //automatically converted to JS list

public slots:
    void addDevice(const Device &device); // TODO - see if can be added from QML?
    void removeDevice(const Device &device);
    void writeChanges();

signals:
    void devicesChanged();

private:
    bool m_changesPending = false;
    QFile *m_configFile = nullptr;
    QJsonObject m_settings;


};

#endif // USERSETTINGSSERVICE_H

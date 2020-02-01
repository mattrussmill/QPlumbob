#ifndef USERSETTINGSSERVICE_H
#define USERSETTINGSSERVICE_H

#include <QObject>
#include <QVariant>
#include <QFile>
#include <QMap>
#include <QJsonObject>


class Device;

class UserSettingsService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant devices READ devices NOTIFY devicesChanged)

    enum DeviceCheckStatus
    {
        UnfamiliarDevice,
        NoDeviceConflict,
        DeviceConflict
    };
public:
    explicit UserSettingsService(QObject *parent = nullptr);
    ~UserSettingsService();
    QVariant devices() const; //automatically converted to JS list

public slots:
    void addDevice(const Device &device);
    void removeDevice(const Device &device);
    void writeChanges();
    QVariantMap getDeviceById(const QString &id);

    void resetCheckedDevices();
    int checkDevice(const QString &id);

signals:
    void devicesChanged();
    void deviceConflict(const QString &id); //TODO - returns device ID in question, when conflict found remove saved device from list or display conflict (gray out known device) -> ask for pin on connect for devices not recognized

private:
    bool m_changesPending = false;
    QFile *m_configFile = nullptr;
    QJsonObject m_settings;
    QMap<QString, bool> m_lookupCounter; //load on startup or on insert

};

#endif // USERSETTINGSSERVICE_H

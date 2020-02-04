#ifndef USERSETTINGSSERVICE_H
#define USERSETTINGSSERVICE_H

#include "bluetoothbase.h"
#include <QObject>
#include <QVariant>
#include <QFile>
#include <QMap>
#include <QJsonObject>


class Device;

class UserSettingsService : public BluetoothBase
{
    Q_OBJECT
    Q_PROPERTY(QVariant devices READ devices NOTIFY devicesChanged)

public:
    enum DeviceLoadedStatus
    {
        DeviceUnknown,
        DeviceEnabled,
        DeviceDisabled,
        DeviceConflict
    };

    explicit UserSettingsService(QObject *parent = nullptr);
    ~UserSettingsService();
    QVariant devices() const; //automatically converted to JS list

public slots:
    void addDevice(const Device &device);
    void removeDevice(const Device &device);
    void writeChanges();
    QVariantMap getDeviceById(const QString &id);

    void resetCheckedDevices();
    DeviceLoadedStatus checkDevice(const QString &id);

signals:
    void devicesChanged();
    void deviceConflict(const QString &id); //TODO - returns device ID in question, when conflict found remove saved device from list or display conflict (gray out known device) -> ask for pin on connect for devices not recognized

private:
    bool m_changesPending = false;
    QFile *m_configFile = nullptr;
    QJsonObject m_settings;
    QMap<QString, DeviceLoadedStatus> m_lookupCounter; //load on startup or on insert

};

#endif // USERSETTINGSSERVICE_H

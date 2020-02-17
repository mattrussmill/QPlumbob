#ifndef USERSETTINGSSERVICE_H
#define USERSETTINGSSERVICE_H

#include "bluetoothbase.h"
#include <QObject>
#include <QVariant>
#include <QString>
#include <QMap>

class Device;

class UserSettingsService : public BluetoothBase
{
    Q_OBJECT

public:
    enum DeviceLoadedStatus
    {
        DeviceUnknown,
        DeviceEnabled,
        DeviceDisabled,
        DeviceConflict
    };

    struct SavedDevice {
        QString name;
        QString address;
        uint16_t pin;
        DeviceLoadedStatus status;
    };

    explicit UserSettingsService(QObject *parent = nullptr);
    ~UserSettingsService();
    void addToSavedDevices(Device &device);
    void removeFromSavedDevices(Device &device);
    QMap<QString, SavedDevice> getDevices() const;

public slots:
    void addDevice(QObject *device);
    void removeDevice(QObject *device);
    void writeChanges();
    void resetCheckedDevices();
    DeviceLoadedStatus checkDevice(const QString &id);

signals:
    void devicesChanged();

private:
    bool m_changesPending = false;
    const QString m_configFile = "/config.json";
    QMap<QString, SavedDevice> m_savedDevices; //load on startup or on insert
};

#endif // USERSETTINGSSERVICE_H

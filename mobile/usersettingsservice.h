#ifndef USERSETTINGSSERVICE_H
#define USERSETTINGSSERVICE_H

#include "bluetoothbase.h"
#include <QObject>
#include <QVariant>
#include <QString>
#include <QList>

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
    bool addToSavedDevices(const Device &device);
    bool removeFromSavedDevices(const Device &device);
    QList<SavedDevice> getDevices() const;

public slots:
    void writeChanges();
    void resetCheckedDevices();
    DeviceLoadedStatus checkDevice(const QString &id);

signals:
    void devicesChanged();

private:
    int findDeviceIndexById(const QString &id);
    bool m_changesPending = false;
    const QString m_configFile = "/config.json";
    QList<SavedDevice> m_savedDevices; //load on startup or on insert
};

#endif // USERSETTINGSSERVICE_H

#ifndef SCANSERVICE_H
#define SCANSERVICE_H

#include <QObject>
#include <QVariant>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include "components/bluetoothbase.h"
class Device;
class DeviceService;
class UserSettingsService;

class ScanService : public BluetoothBase
{
    Q_OBJECT
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout)
    Q_PROPERTY(QVariant devices READ devices NOTIFY devicesChanged)

public:
    ScanService(QObject *parent = nullptr);
    ScanService(DeviceService *deviceService, QObject *parent = nullptr);
    ScanService(DeviceService *deviceService, UserSettingsService *userSettings, QObject *parent = nullptr);
    ~ScanService();
    bool scanning() const;
    int timeout() const;
    void setTimeout(int timeout);
    void setDeviceService(DeviceService *deviceService);
    QVariant devices() const; //automatically converted to JS list

public slots:
    void startScan();
    void stopScan();
    void connectToDevice(const int index);
    void saveDevice(int index);
    void forgetDevice(int index);

signals:
    void scanningChanged();
    void devicesChanged();
    void deviceLookupError();
    void scanStarted();

private slots:
    void addDevice(const QBluetoothDeviceInfo&);
    void scanError(QBluetoothDeviceDiscoveryAgent::Error error);
    void scanFinished();
    int findDeviceById(const QString &id);

private:
    void initializeDeviceList();
    int m_timeout = 10000;
    QList<QObject*> m_devices;
    QBluetoothDeviceDiscoveryAgent *m_deviceDiscoveryAgent;
    DeviceService *m_deviceService = nullptr;
    UserSettingsService *m_userSettings = nullptr;
};

#endif // SCANSERVICE_H

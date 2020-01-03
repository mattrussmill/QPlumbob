#ifndef SCANSERVICE_H
#define SCANSERVICE_H

#include <QObject>
#include <QVariant>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include "bluetoothbase.h"
#include "deviceservice.h"

class ScanService : public BluetoothBase
{
    Q_OBJECT
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout)
    Q_PROPERTY(QVariant devices READ devices NOTIFY devicesChanged)

public:
    ScanService(QObject *parent = nullptr);
    ScanService(DeviceService *deviceService, QObject *parent = nullptr);
    ~ScanService();
    bool scanning() const;
    int timeout() const;
    void setTimeout(int timeout);
    void setDeviceService(DeviceService *deviceService);
    QVariant devices(); //automatically converted to JS list

public slots:
    void startScan();
    void stopScan();
    void connectToDevice(const QString &address); //@TODO make Q_INVOKABLE?

signals:
    void scanningChanged();
    void devicesChanged();
    void deviceLookupError();

private slots:
    void addDevice(const QBluetoothDeviceInfo&);
    void scanError(QBluetoothDeviceDiscoveryAgent::Error error);
    void scanFinished();

private:
    int m_timeout = 10000;
    QList<QObject*> m_devices;
    QBluetoothDeviceDiscoveryAgent *m_deviceDiscoveryAgent;
    DeviceService *m_deviceService;
};

#endif // SCANSERVICE_H

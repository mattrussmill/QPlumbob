#ifndef SCANSERVICE_H
#define SCANSERVICE_H

#include <QObject>
#include <QVariant>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include "bluetoothbase.h"

class ScanService : public BluetoothBase
{
    Q_OBJECT
    Q_PROPERTY(bool scanning READ scanning NOTIFY scanningChanged)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout)
    Q_PROPERTY(QVariant devices READ devices NOTIFY devicesChanged)

public:
    explicit ScanService(QObject *parent = nullptr);
    ~ScanService();
    bool scanning() const;
    int timeout() const;
    void setTimeout(int timeout);
    QVariant devices(); //automatically converted to JS list

public slots:
    void startScan();
    void stopScan();

signals:
    void scanningChanged();
    void devicesChanged();

private slots:
    void addDevice(const QBluetoothDeviceInfo&);
    void scanError(QBluetoothDeviceDiscoveryAgent::Error error);
    void scanFinished();

private:
    int m_timeout = 5000;
    QList<QObject*> m_devices;
    QBluetoothDeviceDiscoveryAgent *m_deviceDiscoveryAgent;


};

#endif // SCANSERVICE_H

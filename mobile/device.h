#ifndef DEVICE_H
#define DEVICE_H

#include <QBluetoothDeviceInfo>
#include <QObject>
#include <QString>

class Device : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ getName NOTIFY deviceChanged)
    Q_PROPERTY(QString address READ getAddress NOTIFY deviceChanged)

public:
    Device(QObject *parent = nullptr);
    Device(const QBluetoothDeviceInfo &device, QObject *parent = nullptr);
    void setDevice(const QBluetoothDeviceInfo &device);
    QString getName() const;
    QString getAddress() const;
    QBluetoothDeviceInfo getDevice() const;

signals:
    void deviceChanged();

private:
    QBluetoothDeviceInfo m_device;
};

#endif // DEVICE_H

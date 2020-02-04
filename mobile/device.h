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
    Q_PROPERTY(bool available READ isAvailable NOTIFY availabilityChanged) //use these properties for cashed devices
    Q_PROPERTY(bool known READ isKnown NOTIFY knownChanged)

public:
    Device(QObject *parent = nullptr);
    Device(const QBluetoothDeviceInfo &device, QObject *parent = nullptr);
    Device(const QBluetoothDeviceInfo &device, bool available, bool known, QObject *parent = nullptr);
    void setDevice(const QBluetoothDeviceInfo &device);
    QString getName() const;
    QString getAddress() const;
    QBluetoothDeviceInfo getDevice() const;
    bool isAvailable() const;
    bool isKnown() const;
    void setKnown(bool deviceKnown);
    void setAvailable(bool deviceAvailable);

signals:
    void deviceChanged();
    void availabilityChanged();
    void knownChanged();

private:
    QBluetoothDeviceInfo m_device;
    bool m_available = true;
    bool m_known = false;
};

#endif // DEVICE_H

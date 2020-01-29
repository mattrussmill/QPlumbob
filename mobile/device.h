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
    Q_PROPERTY(bool known READ isKnown)

public:
    Device(QObject *parent = nullptr);
    Device(const QBluetoothDeviceInfo &device, QObject *parent = nullptr);
    void setDevice(const QBluetoothDeviceInfo &device);
    QString getName() const;
    QString getAddress() const;
    QBluetoothDeviceInfo getDevice() const;
    bool isAvailable() const;
    bool isKnown() const;
    void setKnown(bool cached);
    void setAvailable(bool available);

signals:
    void deviceChanged();
    void availabilityChanged();

private:
    QBluetoothDeviceInfo m_device;
    bool m_available = true;
};

#endif // DEVICE_H

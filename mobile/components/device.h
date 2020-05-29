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
    Q_PROPERTY(QString pin READ getPin WRITE setPin NOTIFY pinChanged)
    Q_PROPERTY(bool available READ isAvailable NOTIFY availabilityChanged)
    Q_PROPERTY(bool known READ isKnown NOTIFY knownChanged)

public:
    Device(QObject *parent = nullptr);
    Device(const QBluetoothDeviceInfo &device, QObject *parent = nullptr);
    Device(const QBluetoothDeviceInfo &device, bool available, bool known, QObject *parent = nullptr);
    Device(const QBluetoothDeviceInfo &device, bool available, bool known, const QString &pin, QObject *parent = nullptr);
    void setDevice(const QBluetoothDeviceInfo &device);
    void setPin(const QString &pin);
    void setKnown(bool deviceKnown);
    void setAvailable(bool deviceAvailable);
    QString getName() const;
    QString getAddress() const;
    QString getPin() const;
    QBluetoothDeviceInfo getDevice() const;
    bool isAvailable() const;
    bool isKnown() const;

signals:
    void deviceChanged();
    void pinChanged();
    void availabilityChanged();
    void knownChanged();

private:
    QBluetoothDeviceInfo m_device;
    QString m_pin;
    bool m_available = true;
    bool m_known = false;
};

#endif // DEVICE_H

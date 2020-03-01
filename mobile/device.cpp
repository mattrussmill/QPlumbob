#include "device.h"
#include <QBluetoothAddress>
#include <QDebug>

Device::Device(QObject *parent) : QObject(parent)
{

}

Device::Device(const QBluetoothDeviceInfo &device, QObject *parent) :
    QObject(parent), m_device(device)
{

}

Device::Device(const QBluetoothDeviceInfo &device, bool available, bool known, QObject *parent) :
    QObject(parent), m_device(device), m_available(available), m_known(known)
{

}

Device::Device(const QBluetoothDeviceInfo &device, bool available, bool known, const QString &pin, QObject *parent) :
    QObject(parent), m_device(device), m_pin(pin), m_available(available), m_known(known)
{

}

void Device::setDevice(const QBluetoothDeviceInfo &device)
{
    m_device = device;
    emit deviceChanged();
}

QString Device::getName() const
{
    return m_device.name();
}

QString Device::getAddress() const
{
    return m_device.address().toString();
}

QBluetoothDeviceInfo Device::getDevice() const
{
    return m_device;
}

QString Device::getPin() const
{
    return m_pin;
}

bool Device::isAvailable() const
{
    return m_available;
}

bool Device::isKnown() const
{
    return m_known;
}

void Device::setPin(const QString &pin)
{
    m_pin = pin;
    emit pinChanged();
}

void Device::setAvailable(bool deviceAvailable)
{
    if (deviceAvailable != m_available)
    {
        m_available = deviceAvailable;
        emit availabilityChanged();
    }
}

void Device::setKnown(bool deviceKnown)
{
    if (deviceKnown != m_known)
    {
        m_known = deviceKnown;
        emit knownChanged();
    }
}

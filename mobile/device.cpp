#include "device.h"
#include <QBluetoothAddress>

Device::Device(QObject *parent) : QObject(parent)
{

}

Device::Device(const QBluetoothDeviceInfo &device, QObject *parent) :
    QObject(parent), m_device(device)
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

bool Device::isAvailable() const
{
    return m_available;
}

bool Device::isKnown() const
{
    return false; // TODO
}

void Device::setAvailable(bool available)
{
    m_available = available;
    emit availabilityChanged();
}

void Device::setKnown(bool cached)
{
    // TODO
}

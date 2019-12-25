#include <QString>
#include "scanservice.h"
#include "device.h"

ScanService::ScanService(QObject *parent) : BluetoothBase(parent)
{
    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    m_deviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(m_timeout);

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &ScanService::addDevice);
    connect(m_deviceDiscoveryAgent, static_cast<void (QBluetoothDeviceDiscoveryAgent::*)(QBluetoothDeviceDiscoveryAgent::Error)>(&QBluetoothDeviceDiscoveryAgent::error),
            this, &ScanService::scanError);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &ScanService::scanFinished);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &ScanService::scanFinished);

}
ScanService::~ScanService()
{
    qDeleteAll(m_devices);
    m_devices.clear();
}

QVariant ScanService::devices()
{
    return QVariant::fromValue(m_devices);
}

bool ScanService::scanning() const
{
    return m_deviceDiscoveryAgent->isActive();
}

void ScanService::addDevice(const QBluetoothDeviceInfo &device)
{
    // If device is LowEnergy-device, add it to the list
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        m_devices.append(new Device(device));
        setInfo(tr("Low Energy device " + device.name().toLatin1() + " found."));
        emit devicesChanged();
    }
}

void ScanService::startSearch()
{
    clearMessages();
    qDeleteAll(m_devices);
    m_devices.clear();

    emit devicesChanged();

    //@TODO

}

void ScanService::stopSearch()
{
    //@TODO

}

void ScanService::scanFinished()
{
    if (m_devices.isEmpty())
        setError(tr("No devices found."));
    else
        setInfo(tr("Scanning done."));

    emit scanningChanged();
}

void ScanService::scanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    switch (error)
    {
        case(QBluetoothDeviceDiscoveryAgent::PoweredOffError):
        {
            setError(tr("The Bluetooth adaptor is powered off."));
            break;
        }
        case(QBluetoothDeviceDiscoveryAgent::InputOutputError):
        {
            setError(tr("Writing or reading from the device resulted in an error."));
            break;
        }
        case(QBluetoothDeviceDiscoveryAgent::InvalidBluetoothAdapterError):
        {
            setError(tr("Local adapter address does not match the physical adapter address of any local device."));
            break;
        }
        case(QBluetoothDeviceDiscoveryAgent::UnsupportedPlatformError):
        {
            setError(tr("Device discovery is not possible or implemented on the current platform."));
            break;
        }
        case(QBluetoothDeviceDiscoveryAgent::UnsupportedDiscoveryMethod):
        {
            setError(tr("Requested discovery methods is not supported by the current platform."));
            break;
        }
        default: {
            setError(tr("An unknown error has occurred."));
        }
    }
}

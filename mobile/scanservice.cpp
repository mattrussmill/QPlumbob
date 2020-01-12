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

ScanService::ScanService(DeviceService *deviceService, QObject *parent) :
    BluetoothBase(parent), m_deviceService(deviceService)
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
    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
        m_devices.append(new Device(device));
        setInfo(tr("Low Energy device " + device.name().toLatin1() + " found."));
        emit devicesChanged();
    }
}

void ScanService::startScan()
{
    if(m_deviceDiscoveryAgent->isActive())
        m_deviceDiscoveryAgent->stop();

    clearMessages();
    qDeleteAll(m_devices);
    m_devices.clear();
    emit devicesChanged();

    m_deviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    emit scanningChanged();
    setInfo(tr("Scanning for devices..."));
}

void ScanService::stopScan()
{
    if(m_deviceDiscoveryAgent->isActive())
        m_deviceDiscoveryAgent->stop();
}

void ScanService::scanFinished()
{
    if (m_devices.isEmpty())
        setError(tr("No devices found."));
    else
        setInfo(tr("Scanning done."));

    emit scanningChanged();
    emit devicesChanged();
}

void ScanService::connectToDevice(const QString &address)
{
    m_deviceDiscoveryAgent->stop();

    Device *currentDevice = nullptr;
    for (QObject *d : qAsConst(m_devices))
    {
        auto device = qobject_cast<Device *>(d);
        if (device && device->getAddress() == address)
        {
            currentDevice = device;
            break;
        }
    }

    if (currentDevice)
    {
        m_deviceService->setDevice(currentDevice);
        clearMessages();
    }
    else
    {
        setError(tr("Unable to connect to device " + address.toLatin1() + ". Device not found."));
        emit deviceLookupError();
    }

}

int ScanService::timeout() const
{
   return m_timeout;
}

void ScanService::setTimeout(int timeout)
{
    if(timeout < 0)
    {
        setError(tr("Invalid scan timeout set: " + QString::number(timeout).toLatin1() + "ms"));
        return;
    }

    m_timeout = timeout;
    setInfo(tr("Scan timeout set: " + QString::number(timeout).toLatin1() + "ms"));
}

void ScanService::setDeviceService(DeviceService *deviceService)
{
    m_deviceService = deviceService;
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
            setError(tr("Writing to or reading from the device resulted in an error."));
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

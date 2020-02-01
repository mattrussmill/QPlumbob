#include <QString>
#include <QList>
#include <QBluetoothAddress>
#include "scanservice.h"
#include "deviceservice.h"
#include "device.h"
#include "usersettingsservice.h"

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

ScanService::ScanService(DeviceService *deviceService, UserSettingsService *userSettings, QObject *parent) :
    BluetoothBase(parent), m_userSettings(userSettings), m_deviceService(deviceService)
{
    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    m_deviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(m_timeout);

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &ScanService::addDevice);
    connect(m_userSettings, &UserSettingsService::deviceConflict, this, &ScanService::handleKnownDeviceConflict);
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

QVariant ScanService::devices() const
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

        int result = m_userSettings->checkDevice(device.address().toString());


        Device *d = new Device(device);

        // TODO check device here if in saved list. If in saved list

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
    initializeDeviceList();

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
        if (device && device->getAddress() == address) //iOS will have to be uuid
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
            setError(tr("The Bluetooth adaptor is powered off."));
            break;
        case(QBluetoothDeviceDiscoveryAgent::InputOutputError):
            setError(tr("Writing to or reading from the device resulted in an error."));
            break;
        case(QBluetoothDeviceDiscoveryAgent::InvalidBluetoothAdapterError):
            setError(tr("Local adapter address does not match the physical adapter address of any local device."));
            break;
        case(QBluetoothDeviceDiscoveryAgent::UnsupportedPlatformError):
            setError(tr("Device discovery is not possible or implemented on the current platform."));
            break;
        case(QBluetoothDeviceDiscoveryAgent::UnsupportedDiscoveryMethod):
            setError(tr("Requested discovery methods is not supported by the current platform."));
            break;
        default:
            setError(tr("An unknown error has occurred."));
    }
}

void ScanService::handleKnownDeviceConflict(const QString &id)
{
    if (m_userSettings == nullptr)
        return;

    for (QList<QObject*>::iterator d = m_devices.begin(); d != m_devices.end(); ++d){
        if(qobject_cast<Device*>(*d)->getAddress() == id && qobject_cast<Device*>(*d)->isKnown())
        {
            qobject_cast<Device*>(*d)->setKnown(false);
            return;
        }
    }
}

void ScanService::initializeDeviceList()
{
    qDeleteAll(m_devices);
    m_devices.clear();

    if(m_userSettings)
    {
        m_userSettings->resetCheckedDevices();

        QBluetoothDeviceInfo device;
        QVariantList savedDevices = m_userSettings->devices().toList();
        for(QVariantList::iterator i = savedDevices.begin(); i != savedDevices.end(); ++i)
        {
            device = QBluetoothDeviceInfo(
                        QBluetoothAddress(i->toMap()["address"].toString()),
                        i->toMap()["name"].toString(),
                        0);
            device.setCoreConfigurations(QBluetoothDeviceInfo::LowEnergyCoreConfiguration);
            m_devices.append(new Device(device));
        }
    }

    emit devicesChanged();
}

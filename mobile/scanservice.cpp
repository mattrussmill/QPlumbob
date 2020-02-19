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
    BluetoothBase(parent), m_deviceService(deviceService), m_userSettings(userSettings)
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
    setInfo(tr("Device ") + device.name() + tr(" found."));

    if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {

        if (m_userSettings)
        {
            int index;
            switch(m_userSettings->checkDevice(device.address().toString()))
            {
                case UserSettingsService::DeviceConflict: //DeviceUnknown
                    // find first device (the conflict) and set to active/unknown, then append the new device
                    index = findDeviceById(device.address().toString());
                    if (index >= 0)
                    {
                        qobject_cast<Device*>(m_devices[index])->setKnown(false);
                        setInfo(device.name() + " & " + qobject_cast<Device*>(m_devices[index])->getName()
                                + tr(" created a device conflict during scan, but it was resolved."));
                    }
                    else
                    {
                        setError(device.name() + tr(" could not be found to resolve search conflict."));
                    }
                    m_devices.append(new Device(device));
                    break;
                case UserSettingsService::DeviceEnabled: //DeviceKnown
                    // find first device and set known device as available (replace device since disabled devices are just placeholders)
                    index = findDeviceById(device.address().toString());
                    if (index >= 0)
                    {
                        Device *d = qobject_cast<Device*>(m_devices[index]);
                        m_devices.replace(index, new Device(device, true, true));
                        delete d;
                        setInfo(device.name() + tr(" marked available as a known device."));
                    }
                    else
                    {
                        setError(device.name() + tr(" could not enabled as a known device."));
                        m_devices.append(new Device(device));
                    }
                    break;
                default: // UserSettingsService::DeviceUnknown
                    // append unrecognized device to list as available and unknown
                    m_devices.append(new Device(device));
                    break;
            }
        }
        else
        {
            m_devices.append(new Device(device));
        }


        emit devicesChanged();
    }
}

void ScanService::startScan()
{
    if(m_deviceDiscoveryAgent->isActive())
        m_deviceDiscoveryAgent->stop();

    emit scanStarted();
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

void ScanService::initializeDeviceList()
{
    qDeleteAll(m_devices);
    m_devices.clear();

    if (m_userSettings)
    {
        m_userSettings->resetCheckedDevices();

        for (QList<UserSettingsService::SavedDevice>::const_iterator i = m_userSettings->getDevices().cbegin();
             i != m_userSettings->getDevices().cend(); ++i)
        {
            setInfo(tr("Saved device added to initialized scan service list: ") + i->name);
            m_devices.append(new Device(
                                    QBluetoothDeviceInfo(
                                        QBluetoothAddress(i->address),
                                            i->name,
                                            0),
                                    false,
                                    true));
        }
    }

    emit devicesChanged();
}

int ScanService::findDeviceById(const QString &id)
{
    for (int i = 0; i < m_devices.length(); i++)
    {
        // find first in list
        if(qobject_cast<Device*>(m_devices[i])->getAddress() == id){
            setInfo(qobject_cast<Device*>(m_devices[i])->getName() + tr(" was found in the scan list."));
            return i;
        }
    }
    setError(tr("Device ") + id + tr(" could not be found in the scan list."));
    return -1;
}

void ScanService::saveDevice(int index)
{
    setInfo(tr("Attempting to save device of index: ") + QString::number(index));
    if (m_userSettings)
    {
        if (index >= 0 && index < m_devices.length())
        {
            Device *d = qobject_cast<Device*>(m_devices[index]);
            if (m_userSettings->addToSavedDevices(*d))
                d->setKnown(true);
        }
        else
        {
            setError(tr("Could not save device, index out of range"));
        }
    }
    else
    {
        setError(tr("Device settings not available, feature not initialized"));
    }
}

void ScanService::forgetDevice(int index)
{
    setInfo(tr("Attempting to forget device of index: ") + QString::number(index));
    if (m_userSettings)
    {
        Device *d = qobject_cast<Device*>(m_devices[index]);
        if (m_userSettings->removeFromSavedDevices(*d))
        {
            if(d->isAvailable())
            {
                d->setKnown(false);
            }
            else
            {
                m_devices.removeAt(index);
                delete d;
                emit devicesChanged();
            }
        }
    }
    else
    {
        setError(tr("Device settings not available, feature not initialized"));
    }
}

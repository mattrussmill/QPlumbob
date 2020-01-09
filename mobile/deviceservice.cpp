#include "deviceservice.h"
#include "device.h"
#include <QBluetoothUuid>

DeviceService::DeviceService(QObject *parent) :
    BluetoothBase(parent), m_availableRegions(0)
{

}

DeviceService::DeviceService(Device *device, QObject *parent) :
    BluetoothBase(parent), m_device(device), m_availableRegions(0)
{

}

DeviceService::~DeviceService()
{

}

void DeviceService::setDevice(Device *device)
{
    clearMessages();
    m_device = device;

    qDebug() << device->getName() << "was passed to setDevice";
    // disconnect and delete old device if exists
    if(m_control)
    {
        m_control->disconnectFromDevice();
        delete m_control;
        m_control = nullptr;
    }

    // valid device, set up controller (central)
    if(m_device)
    {
        m_control = QLowEnergyController::createCentral(m_device->getDevice(), this);

            qDebug() << m_control;

        connect(m_control, &QLowEnergyController::serviceDiscovered, this, &DeviceService::serviceDiscovered);
        connect(m_control, &QLowEnergyController::discoveryFinished, this, &DeviceService::serviceScanFinished);
        connect(m_control, static_cast<void (QLowEnergyController::*)(QLowEnergyController::Error)>(&QLowEnergyController::error),
                this, [this]() {
            setError(m_control->errorString()); // TODO - does this produce the correct errors?
        });
        connect(m_control, &QLowEnergyController::connected, this, [this]() {
            setInfo(tr("BLE controller connected. Search services..."));
            m_control->discoverServices();
        });
        connect(m_control, &QLowEnergyController::disconnected, this, [this]() {
            setError(tr("BLE controller disconnected"));
        });

        m_control->connectToDevice();
    }
}

void DeviceService::serviceDiscovered(const QBluetoothUuid &gatt)
{

    setInfo("Service " + gatt.toString(QUuid::WithoutBraces)
            + " found to be offered by " + m_device->getName()
            + ".");
    //TODO
//    if (gatt == QBluetoothUuid(QBluetoothUuid::HeartRate)) {
//        setInfo("Heart Rate service discovered. Waiting for service scan to be done...");
//        m_foundHeartRateService = true;
//    }
}

void DeviceService::serviceScanFinished()
{
    setInfo("Service scan finished. "
            + QString::number(m_control->services().length())
            + " services found.");

    // delete old service if available
    if (m_service) {
        delete m_service;
        m_service = nullptr;
    }

    //TODO example for predefined services
//    // If heartRateService found, create new service
//    if (m_foundHeartRateService)
//        m_service = m_control->createServiceObject(QBluetoothUuid(QBluetoothUuid::HeartRate), this);

}

//void serviceStateChanged(QLowEnergyService::ServiceState s)
//{
//    //TODO
//}

bool DeviceService::alive() const
{
    //TODO service discovery required
    return false;
}

int DeviceService::availableRegions() const
{
    return -1;
}

int DeviceService::timeoutRemaining() const
{
    return -1;
}

int DeviceService::pelvisDutyCycle() const
{
    return -1;
}

bool DeviceService::pelvisAvailable() const
{
    return -1;
}

int DeviceService::gluteusDutyCycle() const
{
    return -1;
}

bool DeviceService::gluteusAvailable() const
{
    return false;
}

void DeviceService::disconnectService()
{
    m_dutyCycles.clear();
}

void DeviceService::setDutyCycle(uint flag, int percent)
{

}

void DeviceService::setTimeout(int minutes)
{

}

void DeviceService::queryDutyCycleRegions()
{

}

void DeviceService::queryDeviceVersionInfo()
{

}



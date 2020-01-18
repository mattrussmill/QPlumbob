#include <QBluetoothUuid>
#include <QString>
#include "deviceservice.h"
#include "device.h"
#include "../deviceidentifiers.h"


DeviceService::DeviceService(QObject *parent) :
    BluetoothBase(parent)
{

}

DeviceService::DeviceService(Device *device, QObject *parent) :
    BluetoothBase(parent), m_device(device)
{

}

DeviceService::~DeviceService()
{

}

void DeviceService::setDevice(Device *device)
{
    clearMessages();
    m_device = device;

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

    // check if service is expected
    if (gatt == QBluetoothUuid(QLatin1String(DevInfo::GARMENT_SERVICE)))
    {
        setInfo(tr("Garment service discovered."));
        m_foundGarmentService = true;
    }
}

void DeviceService::disconnectDevice()
{
    disconnectServices();

    m_control->disconnectFromDevice();
    delete m_control;
    m_control = nullptr;
}

void DeviceService::disconnectServices()
{
    //TODO setup notifications

    m_foundGarmentService = false;
    if(m_garmentService)
    {
        delete m_garmentService;
        m_garmentService = nullptr;
    }
}

void DeviceService::serviceScanFinished()
{
    setInfo(tr("Service scan finished. "
            + QString::number(m_control->services().length()).toLatin1()
            + " services found."));

    // delete old service if available
    if (m_garmentService) {
        delete m_garmentService;
        m_garmentService = nullptr;
    }

    // setup services if found
    if (m_foundGarmentService)
    {
        m_garmentService = m_control->createServiceObject(QBluetoothUuid(QLatin1String(DevInfo::GARMENT_SERVICE)), this);

        if (m_garmentService)
        {
            connect(m_garmentService, &QLowEnergyService::stateChanged, this, &DeviceService::serviceStateChanged);
            connect(m_garmentService, &QLowEnergyService::characteristicWritten, this, &DeviceService::updateGarmentCharacteristic);
            // TODO add additional signals needed as well as error for characteristicWritten and class
            setInfo(tr("Garment service set."));
        }
    }
}

void DeviceService::updateGarmentCharacteristic(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    if (characteristic.uuid() == QBluetoothUuid(QLatin1String(DevInfo::PELVIS_PWM_CHARACTERISTIC)))
    {
        m_pelvisDutyCycle = *value.data();
        emit pelvisDutyCycleChanged();
    } else if (characteristic.uuid() == QBluetoothUuid(QLatin1String(DevInfo::GLUTEUS_PWM_CHARACTERISTIC)))
    {
        m_gluteusDutyCycle = *value.data();
        emit gluteusDutyCycleChanged();
    }
}

void DeviceService::serviceStateChanged(QLowEnergyService::ServiceState state)
{
//    switch(state)
//    {
//        case QLowEnergyService::DiscoveringServices :
//            setInfo(tr("Discovering services..."));
//            break;
//        case                      //TODO
//    }
}

void DeviceService::serviceError(QLowEnergyService::ServiceError newError)
{
    // TODO
}

bool DeviceService::alive() const
{
    return m_garmentService->state() == QLowEnergyService::ServiceDiscovered;
}

int DeviceService::availableRegions() const
{
    return -1; // TODO
}

int DeviceService::timeoutRemaining() const
{
    return -1; // TODO
}

void DeviceService::setPelvisDutyCyckle(int percent)
{
    setInfo(tr("Attempting to set pelvis pwm to: " + QString::number(percent).toLatin1() + "%"));
    m_garmentService->writeCharacteristic(
                m_garmentService->characteristic(QBluetoothUuid(QLatin1String(DevInfo::PELVIS_PWM_CHARACTERISTIC))),
                QByteArray(1, static_cast<uchar>(percent)),
                QLowEnergyService::WriteMode::WriteWithResponse);
}

int DeviceService::pelvisDutyCycle() const
{
    return m_pelvisDutyCycle;
}

bool DeviceService::pelvisAvailable() const
{
    return m_garmentService->characteristic(QBluetoothUuid(QLatin1String(DevInfo::PELVIS_PWM_CHARACTERISTIC))).isValid();
}

void DeviceService::setGluteusDutyCyckle(int percent)
{
    setInfo(tr("Attempting to set gluteus pwm to: " + QString::number(percent).toLatin1() + "%"));
    m_garmentService->writeCharacteristic(
                m_garmentService->characteristic(QBluetoothUuid(QLatin1String(DevInfo::GLUTEUS_PWM_CHARACTERISTIC))),
                QByteArray(1, static_cast<uchar>(percent)),
                QLowEnergyService::WriteMode::WriteWithResponse);
}

int DeviceService::gluteusDutyCycle() const
{
    return m_gluteusDutyCycle;
}

bool DeviceService::gluteusAvailable() const
{
    return m_garmentService->characteristic(QBluetoothUuid(QLatin1String(DevInfo::GLUTEUS_PWM_CHARACTERISTIC))).isValid();
}

void DeviceService::setTimeout(int minutes)
{
 // TODO
}

void DeviceService::queryDeviceVersionInfo()
{

}



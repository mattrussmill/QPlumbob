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

        connect(m_control, &QLowEnergyController::serviceDiscovered, this, &DeviceService::serviceDiscovered);
        connect(m_control, &QLowEnergyController::discoveryFinished, this, &DeviceService::serviceScanFinished);
        connect(m_control, static_cast<void (QLowEnergyController::*)(QLowEnergyController::Error)>(&QLowEnergyController::error), this, [this]() {
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
    emit deviceChanged();
}

void DeviceService::serviceDiscovered(const QBluetoothUuid &gatt)
{
    setInfo("Service " + gatt.toString(QUuid::WithoutBraces)
            + " found to be offered by " + m_device->getName()
            + ".");

    // check if service is expected
    if (gatt == QBluetoothUuid(QLatin1String(DevInfo::AUTH_SERVICE)))
    {
        setInfo(tr("Authentication service discovered."));
        m_foundAuthService = true;
    }
    else if (gatt == QBluetoothUuid(QLatin1String(DevInfo::GARMENT_SERVICE)))
    {
        setInfo(tr("Garment service discovered."));
        m_foundGarmentService = true;
    }
}

void DeviceService::disconnectDevice()
{
    disconnectServices();

    if(m_control)
    {
        m_control->disconnectFromDevice();
        delete m_control;
        m_control = nullptr;
    }
}

void DeviceService::disconnectServices()
{
    bool serviceDisconnected = false;

    m_foundAuthService = false;
    if(m_authService)
    {
        delete m_authService;
        m_authService = nullptr;
        serviceDisconnected = true;
    }

    m_foundGarmentService = false;
    if(m_garmentService)
    {
        delete m_garmentService;
        m_garmentService = nullptr;
        serviceDisconnected = true;
    }

    if (serviceDisconnected)
        emit aliveChanged();
}

void DeviceService::serviceScanFinished()
{
    setInfo(tr("Service scan finished. "
            + QString::number(m_control->services().length()).toLatin1()
            + " services found."));

    // delete old service if available
    if (m_authService) {
        delete m_authService;
        m_authService = nullptr;
    }

    if (m_garmentService) {
        delete m_garmentService;
        m_garmentService = nullptr;
    }

    // setup services if found
    if (m_foundAuthService)
    {
        m_authService = m_control->createServiceObject(QBluetoothUuid(QLatin1String(DevInfo::AUTH_SERVICE)), this);

        if (m_authService)
        {
            connect(m_authService, &QLowEnergyService::stateChanged, this, [this](QLowEnergyService::ServiceState state) {
                serviceStateChanged(state, m_authService->serviceUuid().toString(QUuid::WithoutBraces));
            });
            connect(m_authService, &QLowEnergyService::stateChanged, this, &DeviceService::authenticate);
            connect(m_authService, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error), this, [this](QLowEnergyService::ServiceError error){
                serviceError(error, m_authService->serviceUuid().toString(QUuid::WithoutBraces));
            });
            connect(m_authService, &QLowEnergyService::characteristicWritten, this,  &DeviceService::updateAuthCharacteristic);

            // TODO -- test how this works INDICATE VS NOTIFY -> also probably want to change to a different function so a notification can occur if device not set to same written value
            //connect(m_garmentService, &QLowEnergyService::characteristicChanged, this, &DeviceService::updateGarmentCharacteristic);

            m_authService->discoverDetails();
            setInfo(tr("Authentication service set."));
        }
    }

    if (m_foundGarmentService)
    {
        m_garmentService = m_control->createServiceObject(QBluetoothUuid(QLatin1String(DevInfo::GARMENT_SERVICE)), this);

        if (m_garmentService)
        {
            connect(m_garmentService, &QLowEnergyService::stateChanged, this, [this](QLowEnergyService::ServiceState state) {
                serviceStateChanged(state, m_garmentService->serviceUuid().toString(QUuid::WithoutBraces));
            });
            connect(m_garmentService, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error), this, [this](QLowEnergyService::ServiceError error){
                serviceError(error, m_garmentService->serviceUuid().toString(QUuid::WithoutBraces));
            });
            connect(m_garmentService, &QLowEnergyService::characteristicWritten, this, &DeviceService::updateGarmentCharacteristic);

            m_garmentService->discoverDetails();
            setInfo(tr("Garment service set."));
        }
    }
}

void DeviceService::updateAuthCharacteristic(const QLowEnergyCharacteristic &characteristic, const QByteArray &value) //TODO keep?
{
    if (characteristic.uuid() == QBluetoothUuid(QLatin1String(DevInfo::PIN_CHARACTERISTIC)))
    {
        setInfo(tr("Attempted pin successfully written"));
    }
}

void DeviceService::updateGarmentCharacteristic(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    if (characteristic.uuid() == QBluetoothUuid(QLatin1String(DevInfo::PELVIS_PWM_CHARACTERISTIC)))
    {
        m_pelvisDutyCycle = *value.data();
        emit pelvisDutyCycleChanged();
        setInfo(tr("Pelvis pwm value successfully written: ") + QString::number(m_pelvisDutyCycle));
    } else if (characteristic.uuid() == QBluetoothUuid(QLatin1String(DevInfo::GLUTEUS_PWM_CHARACTERISTIC)))
    {
        m_gluteusDutyCycle = *value.data();
        emit gluteusDutyCycleChanged();
        setInfo(tr("Gluteus pwm value successfully written: ") + QString::number(m_pelvisDutyCycle));
    }
}

void DeviceService::serviceStateChanged(QLowEnergyService::ServiceState state, const QString &service)
{
    switch(state)
    {
        case QLowEnergyService::InvalidService:
            setInfo(service + tr(" Service State: Invalid service - connection may have been lost."));
            break;
        case QLowEnergyService::DiscoveryRequired:
            setInfo(service + tr(" Service State: Service discovery required."));
            break;
        case QLowEnergyService::DiscoveringServices:
            setInfo(service + tr(" Service State: Discovering services."));
            break;
        case QLowEnergyService::ServiceDiscovered:
            setInfo(service + tr(" Service State: Service discovered."));
            break;
        default:
            setInfo(service + tr(" Service State: Untracked state occured."));
    }
    emit aliveChanged();
}

void DeviceService::serviceError(QLowEnergyService::ServiceError error, const QString &service)
{
    switch(error)
    {
        case QLowEnergyService::OperationError:
            setError(service + tr(": An operation was attempted while the service was not ready."));
            break;
        case QLowEnergyService::CharacteristicReadError:
            setError(service + tr(": An attempt to read a characteristic value failed."));
            break;
        case QLowEnergyService::CharacteristicWriteError:
            setError(service + tr(": An attempt to write a new value to a characteristic failed."));
            break;
        case QLowEnergyService::DescriptorReadError:
            setError(service + tr(": An attempt to read a descriptor value failed. "));
            break;
        case QLowEnergyService::DescriptorWriteError:
            setError(service + tr(": An attempt to write a new value to a descriptor failed."));
            break;
        case QLowEnergyService::UnknownError:
            setError(service + tr(": An unknown error occurred when interacting with the service."));
            break;
        default:
            setError(service + tr(": An untracked error occured."));
    }
}

QVariant DeviceService::device() const
{
    return QVariant::fromValue(qobject_cast<QObject *>(m_device));
}

bool DeviceService::alive() const
{
    if(!(m_garmentService || m_authService))
        return false;

    return m_authService->state() == QLowEnergyService::ServiceDiscovered; //TODO Authenticated - need a bool
}

int DeviceService::timeoutRemaining() const
{
    return -1; // TODO
}

void DeviceService::setAuthenticationPin(const QString &pin)
{
    setInfo(tr("Attempting to set pin to: ") + pin);
    m_authService->writeCharacteristic(
                m_authService->characteristic(QBluetoothUuid(QLatin1String(DevInfo::PIN_CHARACTERISTIC))),
                pin.toLatin1(),
                QLowEnergyService::WriteMode::WriteWithoutResponse); //TODO change to write with response
}

void DeviceService::authenticate(QLowEnergyService::ServiceState state)
{
    if (state == QLowEnergyService::ServiceDiscovered)
    {
        setAuthenticationPin(m_device->getPin());
    }
}

void DeviceService::setPelvisDutyCycle(int percent)
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

void DeviceService::setGluteusDutyCycle(int percent)
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

void DeviceService::queryGarmentService()
{
    // TODO use this at init to get init values if disconnected and reconnected.
}

void DeviceService::queryDeviceVersionInfo()
{
    //TODO
}



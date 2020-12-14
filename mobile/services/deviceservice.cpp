#include <QBluetoothUuid>
#include <QString>
#include "deviceservice.h"
#include "components/device.h"
#include "../../deviceidentifiers.h"


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
            setError(m_control->errorString());
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
    else if (gatt == QBluetoothUuid(QLatin1String(DevInfo::LED_SERVICE)))
    {
        setInfo(tr("LED service discovered."));
        m_foundLedService = true;
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
        //disable notifications before disconnecting
        if (m_authNotificationDescriptor && m_authNotificationDescriptor->isValid()
                && m_authNotificationDescriptor->value() == QByteArray::fromHex("0100"))
        {
            m_authService->writeDescriptor(*m_authNotificationDescriptor, QByteArray::fromHex("0000"));
            setInfo(tr("Authentication notificaton DISABLE service command sent on service disconnect."));
        }

        delete m_authNotificationDescriptor;
        m_authNotificationDescriptor = nullptr;

        delete m_authService;
        m_authService = nullptr;

        serviceDisconnected = true;
    }

    m_foundLedService = false;
    if(m_LedService)
    {
        delete m_LedService;
        m_LedService = nullptr;

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

    if (m_LedService) {
        delete m_LedService;
        m_LedService = nullptr;
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
            connect(m_authService, &QLowEnergyService::stateChanged, this, &DeviceService::authNotifyStateChanged);
            connect(m_authService, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error), this, [this](QLowEnergyService::ServiceError error){
                serviceError(error, m_authService->serviceUuid().toString(QUuid::WithoutBraces));
            });
            connect(m_authService, &QLowEnergyService::characteristicChanged, this,  &DeviceService::updateAuthCharacteristic);
            connect(m_authService, &QLowEnergyService::descriptorWritten, this, &DeviceService::updateAuthDescriptor);

            m_authService->discoverDetails();
            setInfo(tr("Authentication service set."));
        }
    }

    if (m_foundLedService)
    {
        m_LedService = m_control->createServiceObject(QBluetoothUuid(QLatin1String(DevInfo::LED_SERVICE)), this);

        if (m_LedService)
        {
            connect(m_LedService, &QLowEnergyService::stateChanged, this, [this](QLowEnergyService::ServiceState state) {
                serviceStateChanged(state, m_LedService->serviceUuid().toString(QUuid::WithoutBraces));
            });
            connect(m_LedService, QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error), this, [this](QLowEnergyService::ServiceError error){
                serviceError(error, m_LedService->serviceUuid().toString(QUuid::WithoutBraces));
            });
            connect(m_LedService, &QLowEnergyService::characteristicWritten, this, &DeviceService::updateLedCharacteristic);

            m_LedService->discoverDetails();
            setInfo(tr("LED service set."));
        }
    }
}

void DeviceService::updateAuthCharacteristic(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    if (characteristic.uuid() == QBluetoothUuid(QLatin1String(DevInfo::AUTH_STATUS_CHARACTERISTIC)))
    {
        //characteristicWritten or characteristicChanged or characteristicRead?
        const bool receivedValue = *(reinterpret_cast<const bool *>(value.data()));
        if (receivedValue != m_authenticated)
        {
            m_authenticated = receivedValue;
            setInfo(tr("Authentication status changed"));
            emit aliveChanged();
        }
        else
        {
            setInfo(tr("Authentication status pushed but NO change"));
        }
    }
}

// When LED characteristic updated in peripheral, update local value - Arduino in little endian mode, byte swapping not necessary
void DeviceService::updateLedCharacteristic(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    if (characteristic.uuid() == QBluetoothUuid(QLatin1String(DevInfo::HUE_CHARACTERISTIC)))
    {
        m_hueHsvValue = *(reinterpret_cast<const uint8_t*>(value.data()));
        emit hueHsvValueChanged();
        setInfo(tr("Hue HSV value successfully written: ") + QString::number(m_hueHsvValue));

    } else if (characteristic.uuid() == QBluetoothUuid(QLatin1String(DevInfo::SATURATION_CHARACTERISTIC)))
    {
        m_saturationValue = *(reinterpret_cast<const uint8_t*>(value.data()));
        emit saturationValueChanged();
        setInfo(tr("Saturation value successfully written: ") + QString::number(m_saturationValue));
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
    if (m_authenticated)
    {
        if (m_LedService && m_authService)
        {
            return m_authService->state() == QLowEnergyService::ServiceDiscovered
                    && m_LedService->state() == QLowEnergyService::ServiceDiscovered;
        }
    }
    return false;
}

void DeviceService::setAuthenticationPin(const QString &pin)
{
    setInfo(tr("Attempting to set pin to: ") + pin);
    //TODO check characteristic valid first
    m_authService->writeCharacteristic(
                m_authService->characteristic(QBluetoothUuid(QLatin1String(DevInfo::PIN_CHARACTERISTIC))),
                pin.toLatin1(),
                QLowEnergyService::WriteMode::WriteWithoutResponse); //pin is write only
}

// When auth service is discovered, enable client notifications for authentication
void DeviceService::authNotifyStateChanged(QLowEnergyService::ServiceState state)
{
    switch (state)
    {
        case QLowEnergyService::ServiceDiscovered:
        {
            const QLowEnergyCharacteristic authCharacteristic =
                    m_authService->characteristic(QBluetoothUuid(QLatin1String(DevInfo::AUTH_STATUS_CHARACTERISTIC)));
            if (!authCharacteristic.isValid())
            {
                setError("Authentication notification service unavailable!");
                break;
            }

            // delete client config descriptor if exists (shouldn't)
            if (m_authNotificationDescriptor)
                delete m_authNotificationDescriptor;

            m_authNotificationDescriptor = new QLowEnergyDescriptor(
                        authCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration));

            // if descriptor is valid, enable notifications
            if (m_authNotificationDescriptor->isValid()) {
                        m_authService->writeDescriptor(*m_authNotificationDescriptor, QByteArray::fromHex("0100"));
                        setInfo(tr("Authentication notificaton service enable command sent."));
            }
            else
            {
                setError(tr("Authentication notification could NOT be set as descriptor is invalid."));
            }

            break;
        }

        default:
            // do nothing
            break;
    }
}

// confirm that the client configuration descriptor was successfully written
void DeviceService::updateAuthDescriptor(const QLowEnergyDescriptor &descriptor, const QByteArray &value)
{
    if (descriptor.isValid() && descriptor == *m_authNotificationDescriptor)
    {
        if (value == QByteArray::fromHex("0000"))
            setInfo(tr("Authentication notificaton DISABLE service command written successfully."));
        else if (value == QByteArray::fromHex("0100")){
            setInfo(tr("Authentication notificaton ENABLE service command written successfully."));

            // after connecting, set pin once authentication notifications are set
            // as the notification triggers LED ui page access
            setAuthenticationPin(m_device->getPin());
        }
    }
}

void DeviceService::setHueHsvValue(int value)
{
    uint8_t v = static_cast<uint8_t>(value); // input int from qml side
    setInfo(tr("Attempting to set hue HSV value to: " + QString::number(v).toLatin1()));
    if(alive())
    {
        m_LedService->writeCharacteristic(
                    m_LedService->characteristic(QBluetoothUuid(QLatin1String(DevInfo::HUE_CHARACTERISTIC))),
                    QByteArray(sizeof(uint8_t), static_cast<uchar>(v)),
                    QLowEnergyService::WriteMode::WriteWithResponse);
    }
}

void DeviceService::setSaturationValue(int value)
{
    uint16_t v = static_cast<uint8_t>(value); // input int from qml side
    setInfo(tr("Attempting to set saturation value to: " + QString::number(v).toLatin1()));
    if(alive())
    {
        m_LedService->writeCharacteristic(
                    m_LedService->characteristic(QBluetoothUuid(QLatin1String(DevInfo::SATURATION_CHARACTERISTIC))),
                    QByteArray(sizeof(uint8_t), static_cast<uchar>(v)),
                    QLowEnergyService::WriteMode::WriteWithResponse);
    }
}

int DeviceService::hueHsvValue() const
{
    return m_hueHsvValue;
}

int DeviceService::saturationValue() const
{
    return m_saturationValue;
}

bool DeviceService::hueHsvAvailable() const
{
    return m_LedService->characteristic(QBluetoothUuid(QLatin1String(DevInfo::HUE_CHARACTERISTIC))).isValid();
}

bool DeviceService::saturationAvailable() const
{
    return m_LedService->characteristic(QBluetoothUuid(QLatin1String(DevInfo::SATURATION_CHARACTERISTIC))).isValid();
}

void DeviceService::queryLedService()
{
    // TODO use this at init to get init values if disconnected and reconnected.
}



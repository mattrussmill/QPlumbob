#ifndef DEVICESERVICE_H
#define DEVICESERVICE_H

#include <QObject>
#include <QMap>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include "components/bluetoothbase.h"
#include "../../deviceidentifiers.h"

class Device;

class DeviceService : public BluetoothBase
{
    Q_OBJECT

    Q_PROPERTY(bool alive READ alive NOTIFY aliveChanged)
    Q_PROPERTY(QVariant device READ device NOTIFY deviceChanged)

    Q_PROPERTY(int hueHsvValue READ hueHsvValue WRITE setHueHsvValue NOTIFY hueHsvValueChanged)
    Q_PROPERTY(int saturationValue READ saturationValue WRITE setSaturationValue NOTIFY saturationValueChanged)

public:
    DeviceService(QObject *parent = nullptr);
    DeviceService(Device *device, QObject *parent = nullptr);
    ~DeviceService();

    void setDevice(Device *device);
    bool alive() const;
    QVariant device() const;
    void getAuthenticationState() const;

    int hueHsvValue() const;
    int saturationValue() const;
    int valueHsvValue() const;
    int periodValue() const;
    bool hueHsvAvailable() const;
    bool saturationAvailable() const;
    bool valueHsvAvailable() const;
    bool periodAvailable() const;

public slots:
    void disconnectDevice();
    void disconnectServices();

    void setHueHsvValue(int value);
    void setSaturationValue(int value);

    void queryLedService();
    void setAuthenticationPin(const QString &pin);

signals:
    void aliveChanged();
    void safetyTimeoutChanged();
    void reportErrorToUser(QString);
    void deviceChanged();

    void hueHsvValueChanged();
    void saturationValueChanged();

private slots:
    void serviceDiscovered(const QBluetoothUuid &);
    void serviceStateChanged(QLowEnergyService::ServiceState state, const QString &service);
    void serviceError(QLowEnergyService::ServiceError error, const QString &service);
    void serviceScanFinished();
    void updateAuthCharacteristic(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void updateLedCharacteristic(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void updateAuthDescriptor(const QLowEnergyDescriptor &descriptor, const QByteArray &value);
    void authNotifyStateChanged(QLowEnergyService::ServiceState state);

private:
    void updateValue(const QLowEnergyCharacteristic &c,
                              const QByteArray &value); //rename and do something with these
    void confirmedDescriptorWrite(const QLowEnergyDescriptor &d,
                              const QByteArray &value); //rename and do something with these

    QLowEnergyController *m_control = nullptr;
    QLowEnergyService *m_LedService = nullptr;
    QLowEnergyService *m_authService= nullptr;
    QLowEnergyDescriptor *m_authNotificationDescriptor = nullptr;
    Device *m_device = nullptr;
    bool m_foundLedService = false;
    bool m_foundAuthService = false;
    bool m_authenticated = false;

    uint8_t m_hueHsvValue = 0;
    uint8_t m_saturationValue = 0;
};

#endif // DEVICESERVICE_H

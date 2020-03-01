#ifndef DEVICESERVICE_H
#define DEVICESERVICE_H

#include <QObject>
#include <QMap>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include "bluetoothbase.h"
#include "../deviceidentifiers.h"

class Device;

class DeviceService : public BluetoothBase
{
    Q_OBJECT

    Q_PROPERTY(bool alive READ alive NOTIFY aliveChanged)
    Q_PROPERTY(int timeoutRemaining READ timeoutRemaining WRITE setTimeout)
    Q_PROPERTY(QVariant device READ device NOTIFY deviceChanged)

    Q_PROPERTY(int pelvisDutyCycle READ pelvisDutyCycle WRITE setPelvisDutyCyckle NOTIFY pelvisDutyCycleChanged)
    Q_PROPERTY(int gluteusDutyCycle READ gluteusDutyCycle WRITE setGluteusDutyCyckle NOTIFY gluteusDutyCycleChanged)

public:
    DeviceService(QObject *parent = nullptr);
    DeviceService(Device *device, QObject *parent = nullptr);
    ~DeviceService();

    void setDevice(Device *device);
    bool alive() const;
    QVariant device() const;
    int timeoutRemaining() const; //TODO do I need a function for service discovery?

    int pelvisDutyCycle() const;
    bool pelvisAvailable() const;

    int gluteusDutyCycle() const;
    bool gluteusAvailable() const;

public slots:
    void disconnectDevice();
    void disconnectServices();

    void setPelvisDutyCyckle(int percent);
    void setGluteusDutyCyckle(int percent);
    void setTimeout(int minutes);
    void queryGarmentService();
    void queryDeviceVersionInfo();

signals:
    void aliveChanged();
    void safetyTimeoutChanged();
    void reportErrorToUser(QString);
    void deviceChanged();

    void pelvisDutyCycleChanged();
    void pelvisSetDutyCycleFailed();
    void gluteusDutyCycleChanged();
    void gluteusSetDutyCycleFailed();

private slots:
    void serviceDiscovered(const QBluetoothUuid &);
    void serviceStateChanged(QLowEnergyService::ServiceState state);
    void serviceError(QLowEnergyService::ServiceError error);
    void serviceScanFinished();
    void updateGarmentCharacteristic(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);

private:
    void updateValue(const QLowEnergyCharacteristic &c,
                              const QByteArray &value); //rename and do something with these
    void confirmedDescriptorWrite(const QLowEnergyDescriptor &d,
                              const QByteArray &value); //rename and do something with these

    QLowEnergyController *m_control = nullptr;
    QLowEnergyService *m_garmentService = nullptr;
    QLowEnergyService *m_authService= nullptr;
    Device *m_device = nullptr;
    bool m_foundGarmentService = false;
    bool m_foundAuthService = false;

    uint8_t m_pelvisDutyCycle = 0;
    uint8_t m_gluteusDutyCycle = 0;
};

#endif // DEVICESERVICE_H

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
    Q_PROPERTY(int availableRegions READ availableRegions NOTIFY availableRegionsChanged)
    Q_PROPERTY(int timeoutRemaining READ timeoutRemaining WRITE setTimeout)

    Q_PROPERTY(int pelvisDutyCycle READ pelvisDutyCycle NOTIFY pelvisDutyCycleChanged)
    Q_PROPERTY(int gluteusDutyCycle READ gluteusDutyCycle NOTIFY gluteusDutyCycleChanged)

public:
    DeviceService(QObject *parent = nullptr);
    DeviceService(Device *device, QObject *parent = nullptr);
    ~DeviceService();

    void setDevice(Device *device);
    bool alive() const;
    int availableRegions() const;
    int timeoutRemaining() const; //TODO do I need a function for service discovery?

    int pelvisDutyCycle() const;
    bool pelvisAvailable() const;

    int gluteusDutyCycle() const;
    bool gluteusAvailable() const;

public slots:
    void disconnectService();
    void setDutyCycle(uint flag, int percent);
    void setTimeout(int minutes);
    void queryDutyCycleRegions();
    void queryDeviceVersionInfo();

signals:
    void aliveChanged();
    void availableRegionsChanged();
    void safetyTimeoutChanged();
    void reportErrorToUser(QString);

    void pelvisDutyCycleChanged();
    void gluteusDutyCycleChanged();

private slots:
    void serviceDiscovered(const QBluetoothUuid &);
    void serviceScanFinished();
    //void serviceStateChanged(QLowEnergyService::ServiceState s);

private:
    void updateValue(const QLowEnergyCharacteristic &c,
                              const QByteArray &value); //rename and do something with these
    void confirmedDescriptorWrite(const QLowEnergyDescriptor &d,
                              const QByteArray &value); //rename and do something with these

    QLowEnergyController *m_control = nullptr;
    QLowEnergyService *m_service = nullptr;
    QLowEnergyDescriptor m_notificationDesc;
    Device *m_device = nullptr;
    uint m_availableRegions;
    // need version number for device. get device type from UUID lookup in another header file.
    QMap<uint, char> m_dutyCycles;
};

#endif // DEVICESERVICE_H

#ifndef DEVICESERVICE_H
#define DEVICESERVICE_H

#include <QObject>
#include <QMap>
#include <QLowEnergyController>
#include <QLowEnergyService>
#include "bluetoothbase.h"

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
    enum AvailableRegion {
        PELVIS = 0x1,
        GLUTEUS = 0x2
    };
    Q_ENUM(AvailableRegion)

    DeviceService(QObject *parent = nullptr);
    DeviceService(Device *device, QObject *parent = nullptr);
    void setDevice(Device *device);

    bool alive() const;
    int availableRegions() const;
    int timeoutRemaining() const;

    int pelvisDutyCycle() const;
    bool pelvisAvailable() const;

    int gluteusDutyCycle() const;
    bool gluteusAvailable() const;

public slots:
    void disconnectService();
    void setDutyCycle(uint flag, int percent);
    void setTimeout(int minutes);
    void queryRegions();

signals:
    void aliveChanged();
    void availableRegionsChanged();
    void safetyTimeoutChanged();
    void reportError(); //notify user of error

    void pelvisDutyCycleChanged();
    void gluteusDutyCycleChanged();

private:
    QLowEnergyController *m_control = nullptr;
    QLowEnergyService *m_service = nullptr;
    Device *m_device = nullptr;
    uint m_availableRegions = 0;
    QMap<uint, char> m_dutyCycles;
};

#endif // DEVICESERVICE_H

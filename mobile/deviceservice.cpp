#include "deviceservice.h"
#include "device.h"

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
    m_device = device;
}

bool DeviceService::alive() const
{
    //TODO service discovery required
}

int DeviceService::availableRegions() const
{

}

int DeviceService::timeoutRemaining() const
{

}

int DeviceService::pelvisDutyCycle() const
{

}

bool DeviceService::pelvisAvailable() const
{

}

int DeviceService::gluteusDutyCycle() const
{

}

bool DeviceService::gluteusAvailable() const
{

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



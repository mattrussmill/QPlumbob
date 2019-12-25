#ifndef DEVICE_H
#define DEVICE_H

#include <QBluetoothDeviceInfo>
#include <QObject>

class Device : public QObject
{
    Q_OBJECT
public:
    Device(QObject *parent = nullptr);
    Device(const QBluetoothDeviceInfo &device, QObject *parent = nullptr);

signals:

public slots:
};

#endif // DEVICE_H

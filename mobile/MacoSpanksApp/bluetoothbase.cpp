#include <QDebug>
#include "bluetoothbase.h"

BluetoothBase::BluetoothBase(QObject *parent) : QObject(parent)
{

}

BluetoothBase::~BluetoothBase()
{

}

QString BluetoothBase::info() const
{
    return m_info;
}

QString BluetoothBase::error() const
{
    return m_error;
}

void BluetoothBase::setInfo(const QString &info)
{
    if (m_info != info) {
        m_info = info;
        qDebug() << info;
        emit infoChanged();
    }
}

void BluetoothBase::setError(const QString &error)
{
    if (m_error != error) {
        m_error = error;
        qDebug() << error;
        emit errorChanged();
    }
}

void BluetoothBase::clearMessages()
{
    setInfo(QString());
    setError(QString());
}

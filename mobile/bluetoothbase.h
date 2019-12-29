#ifndef BLUETOOTHBASE_H
#define BLUETOOTHBASE_H

#include <QObject>

class BluetoothBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString info READ info WRITE setInfo NOTIFY infoChanged)
    Q_PROPERTY(QString error READ error WRITE setError NOTIFY errorChanged)

public:
    explicit BluetoothBase(QObject *parent = nullptr);
    ~BluetoothBase();

    QString info() const;
    void setInfo(const QString& info);

    QString error() const;
    void setError(const QString& error);

    void clearMessages();

signals:
    void errorChanged();
    void infoChanged();

private:
    QString m_info;
    QString m_error;
};

#endif // BLUETOOTHBASE_H

#include "usersettingsservice.h"
#include <QDebug>
#include <QFile>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "device.h"

UserSettingsService::UserSettingsService(QObject *parent) : BluetoothBase(parent)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if(path.isEmpty())
    {
        setError(tr("Configuration file location inaccessible, no writeable location."));
        return;
    }

    path.append(m_configFile);
    setInfo(tr("Save path set to: ") + path);
    QFile configFile(path, this);
    if(!configFile.exists())
    {
        setError(tr("Settings do not yet exist, using defualt settings."));
    }
    else if (!configFile.open(QFile::ReadOnly))
    {
        setError(tr("Failed to open configuration file."));
    }
    else
    {
        QJsonParseError error;
        QJsonObject jsonLoaded = QJsonDocument::fromJson(configFile.readAll(), &error).object();
        if(error.error == QJsonParseError::NoError)
        {
            // Load initial keys into lookup counter
            if(jsonLoaded.contains("devices") && jsonLoaded.value("devices").isArray())
            {
                QJsonArray devicesLoaded = jsonLoaded.value("devices").toArray();
                for(QJsonArray::iterator i = devicesLoaded.begin(); i!= devicesLoaded.end(); ++i)
                {
                    setInfo(tr("Device loaded from user config: ") + i->toObject().value("address").toString());
                    m_savedDevices.insert(i->toObject().value("address").toString(),
                                          SavedDevice {
                                              i->toObject().value("name").toString(),
                                              i->toObject().value("address").toString(),
                                              static_cast<uint16_t>(i->toObject().value("pin").toInt()),
                                              DeviceDisabled
                                          });
                }
            }

            setInfo(tr("Application settings loaded."));
        }
        else
        {
            setError(error.errorString());
        }
        configFile.close();
    }
}

UserSettingsService::~UserSettingsService()
{

}

void UserSettingsService::addToSavedDevices(Device &device)
{
    if (device.getDevice().isValid())
    {        
        if (m_savedDevices.contains(device.getAddress()))
        {
            setError(tr("Cannot add device to list, duplicate device: ") + device.getName());
            return;
        }

        m_savedDevices.insert(device.getAddress(),
                              SavedDevice {
                                  device.getName(),
                                  device.getAddress(),
                                  0,
                                  DeviceEnabled
                              });
        device.setKnown(true);
        m_changesPending = true;
        setInfo(tr("Device added to saved devices: ") + device.getName());
        emit devicesChanged();
    }
    else
    {
        setError(tr("Could not add device to list, invalid device"));
    }
}

void UserSettingsService::addDevice(QObject *device)
{
    if (device)
        addToSavedDevices(*qobject_cast<Device*>(device));
    else
        setError("Could not add device to list, device pointer null");
}

void UserSettingsService::removeFromSavedDevices(Device &device)
{
    m_savedDevices.remove(device.getAddress());
    device.setKnown(false);
    m_changesPending = true;
    setInfo(tr("Device removed from saved devices: ") + device.getName());
    emit devicesChanged();
}

void UserSettingsService::removeDevice(QObject *device)
{
    if (device)
        removeFromSavedDevices(*qobject_cast<Device*>(device));
    else
        setError("Could not remove device from list, device pointer null");
}

void UserSettingsService::writeChanges()
{
    if (m_changesPending)
    {
        QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        if(path.isEmpty())
        {
            setError(tr("Configuration file location inaccessible, no writeable location."));
            return;
        }

        path.append(m_configFile);
        setInfo(tr("Save path set to: ") + path);
        QFile configFile(path, this);

        if (!configFile.open(QFile::WriteOnly))
        {
               setError(tr("Could not open file to save settings: ") + configFile.errorString());
               return;
        }

        // Save devices
        QJsonArray devices;
        QJsonObject device;
        for (QMap<QString, SavedDevice>::const_iterator i = m_savedDevices.cbegin();
             i != m_savedDevices.cend(); ++i)
        {
            device.insert("name", QJsonValue(i->name));
            device.insert("address", QJsonValue(i->address));
            device.insert("pin", QJsonValue(i->pin));
            devices.append(QJsonValue(device));
        }
        qDebug() << devices;

        // write to file
        QJsonObject settings;
        settings.insert("devices", devices);
        configFile.write(QJsonDocument(settings).toJson());
        configFile.close();
        m_changesPending = false;
        setInfo(tr("Application settings saved."));
    }
}

void UserSettingsService::resetCheckedDevices()
{
    for (QMap<QString, SavedDevice>::iterator i = m_savedDevices.begin();
         i != m_savedDevices.end(); ++i)
    {
        i->status = DeviceDisabled;
    }
}

UserSettingsService::DeviceLoadedStatus UserSettingsService::checkDevice(const QString &id)
{
    if(m_savedDevices.isEmpty())
        return DeviceUnknown;

    switch(m_savedDevices[id].status)
    {
        case DeviceDisabled:
            // mark as enabled, return that device in list should be enabled
            m_savedDevices[id].status = DeviceEnabled;
            return DeviceEnabled;
        case DeviceEnabled:
            // mark as unknown for future scans until conflict resolved, report conflict to be resolved
            m_savedDevices[id].status = DeviceUnknown;
            return DeviceConflict;
        default:
            // if device had conflict report not known so its status isn't changed until reset occurs
            return DeviceUnknown;
    }
}

QMap<QString, UserSettingsService::SavedDevice> UserSettingsService::getDevices() const
{
    return m_savedDevices;
}

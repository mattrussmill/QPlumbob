#include "usersettingsservice.h"
#include <QDebug>
#include <QFile>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include "components/device.h"
#include <QBluetoothAddress>

#if defined (Q_OS_ANDROID)
#include <QtAndroid>
#endif

UserSettingsService::UserSettingsService(QObject *parent) : BluetoothBase(parent)
{
    #if defined(Q_OS_ANDROID)
        // API level 19+, WRITE_EXTERNAL_STORAGE not required for application-specific directories
        QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    #else
        QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    #endif

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
                    m_savedDevices.append(SavedDevice {
                                              i->toObject().value("name").toString(),
                                              i->toObject().value("address").toString(),
                                              i->toObject().value("pin").toString(),
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

bool UserSettingsService::addToSavedDevices(const Device &device)
{
    if (!device.getDevice().isValid())
    {
        setError(tr("Could not add device to list, invalid device"));
        return false;
    }

    if(device.getAddress().isEmpty() || device.getAddress() == "00:00:00:00:00:00")
    {
        setError("Could not add device to list, invalid address");
        return false;
    }

    if(findDeviceIndexById(device.getAddress()) >= 0)
    {
        setError("Could not add device to list, duplicate device");
        return false;
    }

    m_savedDevices.append(SavedDevice {
                              device.getName(),
                              device.getAddress(),
                              device.getPin(),
                              DeviceEnabled
                          });
    m_changesPending = true;
    setInfo(tr("Device added to saved devices: ") + device.getName());
    emit devicesChanged();
    return true;
}

bool UserSettingsService::removeFromSavedDevices(const Device &device)
{

    int i = findDeviceIndexById(device.getAddress());
    if(m_savedDevices.isEmpty() || i < 0)
    {
        setError(tr("Cannot remove device from list, does not exist: ") + device.getName());
        return false;
    }

    m_savedDevices.removeAt(i);
    m_changesPending = true;
    setInfo(tr("Device removed from saved devices: ") + device.getName());
    emit devicesChanged();
    return true;
}

void UserSettingsService::writeChanges()
{
    if (m_changesPending)
    {
        #if defined(Q_OS_ANDROID)
            // API level 19+, WRITE_EXTERNAL_STORAGE not required for application-specific directories
            QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        #else
            QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
        #endif

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
        for (const SavedDevice &savedDevice : m_savedDevices)
        {
            if (!savedDevice.address.isEmpty() && savedDevice.address != "00:00:00:00:00:00")
            {
                device.insert("name", QJsonValue(savedDevice.name));
                device.insert("address", QJsonValue(savedDevice.address));
                device.insert("pin", QJsonValue(savedDevice.pin));
                devices.append(QJsonValue(device));
            }
            else
            {
                setError("Invalid device skipped when writing to saved devices config file");
            }
        }
        // qDebug() << devices;

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
    for (SavedDevice &savedDevice : m_savedDevices)
    {
        savedDevice.status = DeviceDisabled;
    }
}

UserSettingsService::DeviceLoadedStatus UserSettingsService::checkDevice(const QString &id)
{
    int i = findDeviceIndexById(id);
    if(m_savedDevices.isEmpty() || i < 0)
        return DeviceUnknown;

    switch(m_savedDevices[i].status)
    {
        case DeviceDisabled:
            // mark as enabled, return that device in list should be enabled
            m_savedDevices[i].status = DeviceEnabled;
            return DeviceEnabled;
        case DeviceEnabled:
            // mark as unknown for future scans until conflict resolved, report conflict to be resolved
            m_savedDevices[i].status = DeviceUnknown;
            return DeviceConflict;
        default:
            // if device had conflict report not known so its status isn't changed until reset occurs
            return DeviceUnknown;
    }
}

QList<UserSettingsService::SavedDevice> UserSettingsService::getDevices() const
{
    return m_savedDevices;
}

int UserSettingsService::findDeviceIndexById(const QString &id)
{
    for(int i = 0; i < m_savedDevices.length(); i++)
    {
        if(m_savedDevices[i].address == id)
            return i;
    }
    return -1;
}

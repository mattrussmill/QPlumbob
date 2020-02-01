#include "usersettingsservice.h"
#include <QDebug>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonArray>
#include "device.h"

UserSettingsService::UserSettingsService(QObject *parent) : QObject(parent)
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if(path.isEmpty())
    {
        qCritical() << tr("Configuration file location inaccessible, no writeable location.");
        return;
    }

    path.append("/config.json");
    m_configFile = new QFile(path, this);
    if(!m_configFile->exists())
    {
        qWarning() << tr("Settings do not yet exist, using defualt settings.");
    }
    else if (!m_configFile->open(QFile::ReadOnly))
    {
        qWarning() << tr("Failed to open configuration file.");
    }
    else
    {
        QJsonParseError error;
        m_settings = QJsonDocument::fromJson(m_configFile->readAll(), &error).object();
        if(error.error == QJsonParseError::NoError)
        {
            // Load initial keys into lookup counter
            if(m_settings.contains("devices") && m_settings["devices"].isArray())
            {
                for(QJsonArray::iterator i = m_settings["devices"].toArray().begin();
                    i!= m_settings["devices"].toArray().end(); ++i)
                {
                    m_lookupCounter.insert(i->toObject()["address"].toString(), false);
                }
            }

            qInfo() << tr("Application settings loaded.");
        }
        else
        {
            qWarning() << error.errorString();
        }
        m_configFile->close();
    }
}

UserSettingsService::~UserSettingsService()
{

}

QVariant UserSettingsService::devices() const
{
    QVariantList variantList;
    if(m_settings.contains("devices") && m_settings["devices"].isArray())
    {
        // convert to QML recognized containers
        for(QJsonArray::iterator i = m_settings["devices"].toArray().begin();
            i!= m_settings["devices"].toArray().end(); ++i)
        {
            variantList.append(i->toObject().toVariantMap());
        }
    }
    return variantList;
}

void UserSettingsService::addDevice(const Device &device)
{
    if (device.getDevice().isValid())
    {
        // create empty list if does not exist
        if(!(m_settings.contains("devices") && m_settings["devices"].isArray()))
        {
            m_settings["devices"] = QJsonArray();
        }

        // check if device already in list        
        for(QJsonArray::iterator i = m_settings["devices"].toArray().begin();
            i!= m_settings["devices"].toArray().end(); ++i)
        {
            if (i->toObject()["address"] == device.getAddress())
            {
                qWarning() << tr("Cannot add device to list, duplicate device.");
                return;
            }
        }

        //Add to Json Object
        QJsonObject newDevice;
        newDevice["name"]       = device.getName();
        newDevice["address"]    = device.getAddress();
        newDevice["pin"]        = 0;                    // TODO - assign pin
        m_settings["devices"].toArray().append(newDevice);

        //Add to checked list for QML ListView saved device
        m_lookupCounter.insert(device.getAddress(), false);

        emit devicesChanged();
        m_changesPending = true;
    }
}

void UserSettingsService::removeDevice(const Device &device)
{
    if(m_settings.contains("devices") && m_settings["devices"].isArray())
    {
        for(QJsonArray::iterator i = m_settings["devices"].toArray().begin();
            i!= m_settings["devices"].toArray().end(); ++i)
        {
            if (i->toObject()["address"] == device.getAddress())
            {
                // Json Object
                m_settings["devices"].toArray().erase(i);

                // QML ListView saved device
                m_lookupCounter.remove(device.getAddress());

                emit devicesChanged();
                m_changesPending = true;
                qInfo() << tr("Device removed from saved devices.");
                return;
            }
        }
    }
}

void UserSettingsService::writeChanges()
{
    if (m_changesPending && m_configFile != nullptr)
    {
        if (!m_configFile->open(QFile::WriteOnly))
        {
               qWarning() << tr("Could not save application settings.");
               return;
        }
        m_configFile->write(QJsonDocument(m_settings).toJson());
        m_changesPending = false;
        qInfo() << tr("Application settings saved.");
    }
}

void UserSettingsService::resetCheckedDevices()
{
    for (QMap<QString, bool>::iterator i = m_lookupCounter.begin();
         i != m_lookupCounter.end(); ++i)
    {
        i.value() = false;
    }
}

int UserSettingsService::checkDevice(const QString &id) //TODO change to use enums
{
    if (getDeviceById(id).isEmpty() || !m_lookupCounter.contains(id))
        return UnfamiliarDevice;

    if (!m_lookupCounter[id])
    {
        m_lookupCounter[id] = true;
        return NoDeviceConflict;
    }

    emit deviceConflict(id);
    return DeviceConflict;
}

QVariantMap UserSettingsService::getDeviceById(const QString &id)
{
    if(m_settings.contains("devices") && m_settings["devices"].isArray())
    {
        for(QJsonArray::iterator i = m_settings["devices"].toArray().begin();
            i!= m_settings["devices"].toArray().end(); ++i)
        {
            if (i->toObject()["address"] == id)
            {
                qInfo() << tr("Device iterator found in settings JSON object.");
                return i->toObject().toVariantMap();
            }
        }
    }
    qWarning() << tr("Device iterator not found.");
    return QVariantMap();

    // TODO this will be used in QML side. If device is known and available, will return saved device info for pin.
}

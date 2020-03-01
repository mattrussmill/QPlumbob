import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Popup {
    id: root
    modal: true
    property int deviceIndex: -1
    function setSelectedDeviceIndex (index) {
        deviceIndex = index
    }

    onDeviceIndexChanged: {
        if(deviceIndex >= 0)
            saveCheckbox.checked = scanService.devices[deviceIndex].known
    }

    onClosed: {
        userSettings.writeChanges();
    }

    GridLayout {
        id: layout
        anchors.fill: parent
        columns: 2

        CheckBox {
            id: saveCheckbox
            enabled: deviceIndex >= 0 && (saveCheckbox.checked || deviceService.alive)

            onEnabledChanged: {
                if(!enabled)
                    checked = false
            }

            onCheckedChanged: {
                if(enabled) {
                    if (checked)
                        scanService.saveDevice(deviceIndex)
                    else
                        scanService.forgetDevice(deviceIndex)
                }
            }
        }

        Label {
            id: saveLabel
            enabled: deviceIndex >= 0 && (saveCheckbox.checked || deviceService.alive)
            text: "Save Device"
        }
    }
}

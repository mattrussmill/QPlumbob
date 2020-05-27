import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Popup {
    id: root
    modal: true
    property int deviceIndex: -1
    signal openPinDialog()
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

        CheckBox { //TODO use tool button and replace with icon
            id: saveCheckbox
            enabled: deviceIndex >= 0 && (saveCheckbox.checked || deviceService.alive)
            Layout.fillHeight: true
            Layout.fillWidth: true
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

        ToolButton { //TODO make transparent and use pushpin icon
            id: pinButton
            enabled: deviceIndex >= 0 && deviceService.alive
            text: "*"
            Layout.fillHeight: true
            Layout.fillWidth: true
            onClicked: {
                root.close()
                openPinDialog()
            }
        }

        Label {
            id: pinLabel
            enabled: deviceIndex >= 0 && deviceService.alive
            text: "Change Pin"
        }
    }

}

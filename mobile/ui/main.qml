import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

ApplicationWindow {
    id: root
    visible: true
    title: qsTr('MacoMobile')

    header: TitleBar {
        id: titleBar
        Component.onCompleted: {
            backButtonClicked.connect(stack.pop)
            backButtonClicked.connect(scanService.stopScan)
            menuButtonClicked.connect(settingsMenu.open)
        }
    }

    //closing handler to solve android native back button causing crash
    onClosing: {
        if (Qt.platform.os == "android") {
            close.accepted = false
                if (stack.depth > 1) {
                    stack.pop()
                } else {
                    scanService.stopScan()
                }
        }
    }

    SettingsMenu {
        id: settingsMenu
        x: parent.width - settingsMenu.width
    }

    NotifyErrorDialog {
        id: notifyErrorDialog
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        Component.onCompleted: {
            scanService.errorChanged.connect(() => { notifyErrorDialog.openError(scanService) })
            deviceService.errorChanged.connect(() => { notifyErrorDialog.openError(deviceService) })
            userSettings.errorChanged.connect(() => { notifyErrorDialog.openError(userSettings)} )
        }
    }


    StackView {
        id: stack
        initialItem: deviceListComponent
        anchors.fill: parent
        property bool ledLoaded: false

        Component.onCompleted: {
            deviceService.onAliveChanged.connect(ledMenuActions)
        }

        function ledMenuActions(){
            if(deviceService.alive && !ledLoaded){
                stack.push(ledComponent)
            } else if (!deviceService.alive && ledLoaded){
                stack.clear()
            }
        }

        Component {
            id: deviceListComponent

            DeviceListMenu {
                id: deviceList
                Component.onCompleted: {
                    selectedDeviceIndex.connect(settingsMenu.setSelectedDeviceIndex)
                }
            }
        }

        Component {
            id: ledComponent

            LedMenu {
                id: selectedLed
                Component.onCompleted: {
                    stack.ledLoaded = true
                    settingsMenu.openPinDialog.connect(changeDevicePin)
                }
                Component.onDestruction: {
                    stack.ledLoaded = false
                    deviceService.disconnectDevice()
                }
            }
        }
    }
}

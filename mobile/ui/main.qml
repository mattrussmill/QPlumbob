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

    StackView {
        id: stack
        initialItem: deviceListComponent
        anchors.fill: parent
        property bool garmentLoaded: false

        Component.onCompleted: {
            deviceService.onAliveChanged.connect(garmentMenuActions)
        }

        function garmentMenuActions(){
            if(deviceService.alive && !garmentLoaded){
                stack.push(garmentComponent)
            } else if (!deviceService.alive && garmentLoaded){
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
            id: garmentComponent

            GarmentMenu {
                id: selectedGarment
                Component.onCompleted: {
                    stack.garmentLoaded = true
                    settingsMenu.openPinDialog.connect(changeDevicePin)
                }
                Component.onDestruction: {
                    stack.garmentLoaded = false
                    deviceService.disconnectDevice()
                }
            }
        }
    }
}

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

ApplicationWindow {
    visible: true
    title: qsTr('MacoSpanks')

    header: TitleBar {
        id: titleBar
        Component.onCompleted: {
            backButtonClicked.connect(stack.pop)
            backButtonClicked.connect(scanService.stopScan)
            backButtonClicked.connect(deviceService.disconnectDevice)
            menuButtonClicked.connect(settings.open)
        }
        Material.background: deviceService.alive ? 'blue' : 'red' //TODO remove - used for testing
    }

    SettingsMenu {
        id: settings
        x: parent.width - settings.width
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
                stack.pop()
            }
        }

        Component {
            id: deviceListComponent

            DeviceList {
                id: deviceList
            }
        }

        Component {
            id: garmentComponent

            GarmentMenu {
                id: selectedGarment
                Component.onCompleted: {
                    stack.garmentLoaded = true
                }
                Component.onDestruction: {
                    stack.garmentLoaded = false
                }
            }
        }
    }
}

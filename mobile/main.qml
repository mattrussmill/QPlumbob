import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

ApplicationWindow {
    visible: true
    title: qsTr('MacoSpanks')

    Component.onCompleted: {
        deviceService.onAliveChanged.connect(() => console.log('ALIVE CHANGED')) //TODO add function to push menu
    }

    header: TitleBar {
        id: titleBar
        Component.onCompleted: {
            backButtonClicked.connect(stack.pop)
            backButtonClicked.connect(scanService.stopScan)
            backButtonClicked.connect(deviceService.disconnectDevice)
        }
        Material.background: deviceService.alive ? 'blue' : 'red' //TODO remove - used for testing
    }

    function loadGarmentMenu(){
        console.log('LOAD GARMENT MENU');
    }

    StackView {
        id: stack
        initialItem: deviceListComponent
        anchors.fill: parent

        Component {
            id: deviceListComponent

            DeviceList {
                id: deviceList
            }
        }

        Component {
            id: selectedGarmentComponent

            GarmentMenu {
                id: selectedGarment
                //TODO push 'on alive changed'
            }
        }
    }
}

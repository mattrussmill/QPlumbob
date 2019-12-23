import QtQuick 2.12
import QtQuick.Controls 2.12

ApplicationWindow {
    visible: true
    title: qsTr('MacoSpanks')

    header: TitleBar {
        // signals .. slots ..
    }

    StackView {
        id: stack
        initialItem: deviceListComponent
        anchors.fill: parent

        Component {
            id: deviceListComponent

            DeviceList {
                id: deviceList
                anchors.fill: parent
            }
        }

        Component {
            id: selectedGarmentComponent

            GarmentMenu {
                id: selectedGarment
                anchors.fill: parent
            }
        }

        Component.onCompleted: {
            stack.push(selectedGarmentComponent, {'anchors.fill': 'parent'})
        }
    }
}

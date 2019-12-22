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
        initialItem: deviceList
        anchors.fill: parent

        DeviceList{
            id: deviceList
        }


//        Component {
//            id: connectedDevice

//        }
    }
}

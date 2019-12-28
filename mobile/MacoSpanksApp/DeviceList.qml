import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtBluetooth 5.12

Item {
    id: root

    ListView {
        id: list
        anchors.fill: parent
        model: scanService.devices
        visible: count > 0
        delegate: DeviceListDelegate {
            deviceName: name
            deviceAddress: address
            delegateIndex: index
        }
    }

    Label {
        text: qsTr('No Devices Found')
        visible: list.count == 0 && !list.visible
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -controls.height / 2
    }

    ProgressBar {
        visible: scanService.scanning
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: controls.top
        indeterminate: true
    }

    ListControlBar {
        id: controls
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom

        Component.onCompleted: {
            scanClicked.connect(scanService.startScan)
            scanService.startScan()
        }
    }
}

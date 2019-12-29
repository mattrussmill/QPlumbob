import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtBluetooth 5.12

Item {
    id: root

    signal deviceConnected() // will pass device info?

    ListView {
        id: list
        anchors.fill: parent
        model: scanService.devices
        visible: count > 0
        delegate: DeviceListDelegate {
            deviceName: modelData.name
            deviceAddress: modelData.address
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
            connectClicked.connect(scanService.stopScan)
            connectClicked.connect(deviceConnected) //@TODO temporary
            scanService.startScan()
        }
    }
}

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Item {
    id: root

    function unselectDevices() {
        list.selectedIndex = -1
    }

    ListView {
        id: list
        anchors.fill: parent
        model: scanService.devices
        property int selectedIndex: -1
        visible: count > 0
        delegate: DeviceListDelegate {
            deviceName: modelData.name
            deviceAddress: modelData.address
            delegateIndex: index
            selectedDelegateInList: list.selectedIndex //use property binding to deselect items
            Component.onCompleted: {
                deviceSelected.connect(list.setSelectedDevice)
            }
        }

        signal deselectDevices(int index)

        function setSelectedDevice(index) {
            selectedIndex = index
        }

        function connectToDevice() {
            if (selectedIndex < 0) {
                console.log('Invalid device selected')
                return;
            }
            scanService.connectToDevice(scanService.devices[selectedIndex].address)
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
        connectEnabled: list.selectedIndex >= 0

        Component.onCompleted: {
            scanClicked.connect(scanService.startScan)
            scanClicked.connect(() => list.setSelectedDevice(-1))
            connectClicked.connect(scanService.stopScan)
            connectClicked.connect(list.connectToDevice)
            scanService.startScan()
        }
    }
}

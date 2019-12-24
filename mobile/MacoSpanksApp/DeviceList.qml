import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtBluetooth 5.12

Item {
    id: root
    property int scanDuration: 3500

    BluetoothDiscoveryModel {
        id: btModel
        running: false
        discoveryMode: BluetoothDiscoveryModel.DeviceDiscovery
        onDeviceDiscovered: console.log("New device: " + device)
        onRunningChanged: console.log('running: ' + running)
        onErrorChanged: {
                switch (btModel.error) {
                case BluetoothDiscoveryModel.PoweredOffError:
                    console.log("Error: Bluetooth device not turned on")
                    break
                case BluetoothDiscoveryModel.InputOutputError:
                    console.log("Error: Bluetooth I/O Error")
                    break
                case BluetoothDiscoveryModel.InvalidBluetoothAdapterError:
                    console.log("Error: Invalid Bluetooth Adapter Error")
                    break
                case BluetoothDiscoveryModel.NoError:
                    break
                default:
                    console.log("Error: Unknown Error")
                    break
                }
        }
   }

    ListView {
        id: list
        anchors.fill: parent
        model: btModel
        visible: !btModel.running && count > 1
        delegate: Rectangle {
            id: btDelegate
            width: parent.width
            height: column.height + 10

            property bool expended: false;
            clip: true

            Column {
                id: column
                anchors.left: bticon.right
                anchors.leftMargin: 5
                Text {
                    id: bttext
                    text: deviceName ? deviceName : name
                    font.family: "FreeSerif"
                    font.pointSize: 16
                }

                Text {
                    id: details
                    function get_details(s) {
                        if (btModel.discoveryMode == BluetoothDiscoveryModel.DeviceDiscovery) {
                            //We are doing a device discovery
                            var str = "Address: " + remoteAddress;
                            return str;
                        } else {
                            var str = "Address: " + s.deviceAddress;
                            if (s.serviceName) { str += "<br>Service: " + s.serviceName; }
                            if (s.serviceDescription) { str += "<br>Description: " + s.serviceDescription; }
                            if (s.serviceProtocol) { str += "<br>Protocol: " + s.serviceProtocol; }
                            return str;
                        }
                    }
                    visible: opacity !== 0
                    opacity: btDelegate.expended ? 1 : 0.0
                    text: get_details(service)
                    font.family: "FreeSerif"
                    font.pointSize: 14
                    Behavior on opacity {
                        NumberAnimation { duration: 200}
                    }
                }
            }
            Behavior on height { NumberAnimation { duration: 200} }

            MouseArea {
                anchors.fill: parent
                onClicked: btDelegate.expended = !btDelegate.expended
            }
        }
    }

    Label {
        text: qsTr('No Devices Found')
        visible: list.count == 0 && !btModel.running
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -controller.height / 2
    }

    BusyIndicator {
        running: !list.visible && btModel.running
        anchors.centerIn: parent
        anchors.verticalCenterOffset: -controller.height / 2

    }

    // Controls the btModel scanning functionality
    ListControlBar {
        id: controller
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        scanDuration: root.scanDuration

        Component.onCompleted: {
            scan.connect((scanMode) => (btModel.running = scanMode))
            triggerScan()
        }
    }
}

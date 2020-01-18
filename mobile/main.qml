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
        }
    }

    StackView {
        id: stack
        initialItem: deviceListComponent
        anchors.fill: parent

//        Component.onCompleted: {
//            deviceService.onAliveChanged.connect(() => {
//                                                     console.log('@@@@@@@@@@@@@@@@@@')
//                                       if(deviceService.alive)
//                                          console.log('CONNECTED')
//                                       else
//                                           console.log('DISCONNECTED')
//                                               } )
//        }

        Component {
            id: deviceListComponent

            DeviceList {
                id: deviceList

                Component.onCompleted: {
                    titleBar.backButtonClicked.connect(unselectDevices)
                    //deviceConnected.connect(() => stack.push(selectedGarmentComponent))
                }
            }
        }

        Component {
            id: selectedGarmentComponent

            GarmentMenu {
                id: selectedGarment
                //push 'on alive changed'
            }
        }
    }
}

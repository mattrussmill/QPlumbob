import QtQuick 2.12
import QtQuick.Controls 2.12

ApplicationWindow {
    visible: true
    title: qsTr('MacoSpanks')


    header: TitleBar {
        id: titleBar
        Component.onCompleted: {
            backButtonClicked.connect(stack.pop)
        }
    }

    StackView {
        id: stack
        initialItem: deviceListComponent
        anchors.fill: parent

        Component {
            id: deviceListComponent

            DeviceList {
                id: deviceList

                Component.onCompleted: {
                    deviceConnected.connect(() => stack.push(selectedGarmentComponent))
                }
            }
        }

        Component {
            id: selectedGarmentComponent

            GarmentMenu {
                id: selectedGarment
            }
        }
    }
}

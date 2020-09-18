import QtQuick 2.12
import QtQuick.Controls 2.12
import 'qrc:/ui/popups'
import 'qrc:/ui/components'
import 'qrc:/ui/pages'

Item {
    id: root

    function changeDevicePin() {
        userPinDialog.open()
    }

    SwipeView {
        id: view
        currentIndex: 0
        anchors.fill: parent

        ColorSelector {
            id: hueSaturationSelector
            width: width
            height: height

        }

        LedPage {
            id: hueHsvValue
            attributeName: 'Hue'
            onValueChanged: {
                deviceService.setHueHsvValue(value)
            }
        }

        LedPage {
            id: valueHsvValue
            attributeName: 'Value'
            onValueChanged: {
                deviceService.setValueHsvValue(value)
            }
        }

    }

    PageIndicator {
        id: indicator
        count: view.count
        currentIndex: view.currentIndex
        anchors.bottom: view.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }

    PinDialog {
        id: userPinDialog
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        onAccepted: {
            // dialog saves new pin (peripheral already authenticated)
            deviceService.setAuthenticationPin(pin)
        }
    }
}

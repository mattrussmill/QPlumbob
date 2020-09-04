import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    id: root

    function changeDevicePin() {
        userPinDialog.open()
    }

    SwipeView {
        id: view
        currentIndex: 0
        anchors.fill: parent

        LedPage {
            id: hueHsvValue
            attributeName: 'Hue'
            onValueChanged: {
                deviceService.setHueHsvValue(value)
            }
        }

        LedPage {
            id: phaseValue
            attributeName: 'Phase'
            onValueChanged: {
                deviceService.setPhaseValue(value)
            }
        }

        LedPage {
            id: valueHsvValue
            attributeName: 'Value'
            onValueChanged: {
                deviceService.setValueHsvValue(value)
            }
        }

        LedPage {
            id: periodValue
            attributeName: 'Period'
            onValueChanged: {
                deviceService.setPeriodValue(value)
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

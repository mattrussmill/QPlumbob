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

    Rectangle {
        id: colorPreview
        color: Qt.hsva(hueSaturationSelector.hue / 360.0,
                       hueSaturationSelector.saturation / 255.0, 1, 1)
        height: parent.height * 0.05
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
    }

    ColorSelector {
        id: hueSaturationSelector
        height: parent.height - colorPreview.height
        width: parent.width
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        onSaturationChanged: {
            deviceService.setSaturationValue(saturation)
        }

        onHueChanged: {
            deviceService.setHueHsvValue(hue)
        }
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

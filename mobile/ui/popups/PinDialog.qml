import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Dialog {
    id: root
    property string pin
    modal: true
    title: qsTr("Device Pin")
    standardButtons: Dialog.Ok
    onOpened: {
        pinField.clear()
        pin = ""
    }
    onRejected: pin = ""

    ColumnLayout {
        TextField {
            id: pinField
            inputMethodHints: Qt.ImhDigitsOnly | Qt.ImhSensitiveData
            validator: IntValidator {}
            maximumLength: 6
            echoMode: TextInput.Password
            onEditingFinished: root.pin = text
        }
    }
}

import QtQuick 2.12
import QtQuick.Controls 2.12

Dialog {
    id: root
    property bool disconnectOnError: false
    modal: true
    title: qsTr("Device Error")
    standardButtons: Dialog.Ok

    onClosed: {
        error = ""
    }

    function openError(baseObject) {
        if (typeof(baseObject) == "undefined" || !baseObject)
            return
        if (disconnectOnError)
            deviceService.disconnectDevice()
        errorField.text = baseObject.error
        root.open()
    }

    Text {
        id: errorField
        anchors.centerIn: parent
    }
}

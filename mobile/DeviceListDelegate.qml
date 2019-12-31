import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    id: root
    width: parent.width
    height: column.implicitHeight
    //clip: true
    property string deviceName
    property string deviceAddress
    property bool expended: false
    property int delegateIndex

    Column {
        id: column
        anchors.fill: parent
        Label {
            id: name
            text: deviceName ? deviceName : qsTr('Device Name Unavailable ') + delegateIndex
            font.pointSize: 16
        }

        Label {
            id: details
            visible: opacity !== 0
            opacity: root.expended ? 1 : 0.0
            text: deviceAddress
            font.pointSize: 14
            Behavior on opacity {
                NumberAnimation { duration: 200}
            }
        }
    }

    Behavior on height { NumberAnimation { duration: 200} }

    MouseArea {
        anchors.fill: parent
        onClicked: root.expended = !root.expended
    }
}

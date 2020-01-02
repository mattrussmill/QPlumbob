import QtQuick 2.12
import QtQuick.Controls 2.12

ItemDelegate {
    id: root
    width: parent.width
    height: column.implicitHeight
    highlighted: false
    property string deviceName
    property string deviceAddress
    property bool expended: false
    property int delegateIndex

    signal deviceSelected(int index)

    function deselectDevice(index) {
        if(index === delegateIndex) {
            root.expended = false
            highlighted = false
        }
    }

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

    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.expended = !root.expended
            root.highlighted = !root.highlighted

            if(root.expended) {
                deviceSelected(root.delegateIndex);
            } else {
                deviceSelected(-1);
            }
        }
    }
}
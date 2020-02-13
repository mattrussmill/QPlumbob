import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Extras 1.4

ItemDelegate {
    id: root
    width: parent.width
    height: column.implicitHeight
    highlighted: false
    property string deviceName
    property string deviceAddress
    property bool expanded: false
    property bool delegateAvailable
    property bool delegateKnown
    property int delegateIndex
    property int selectedDelegateInList

    signal deviceSelected(int index)

    onSelectedDelegateInListChanged: {
        if(selectedDelegateInList != delegateIndex) {
            root.expanded = false;
            root.highlighted = false;
        }
    }

    Row {
        id: row
        anchors.fill: parent

        StatusIndicator {
            id: indicator
            anchors.centerIn: parent.Center
            active: delegateAvailable
            color: delegateKnown ? "green" : "orange"
        }

        Column {
            id: column
            anchors.centerIn: parent.Center

            Label {
                id: name
                text: deviceName ? deviceName : qsTr('Device Name Unavailable ') + delegateIndex
                font.pointSize: 16
            }

            Label {
                id: details
                visible: opacity !== 0
                opacity: root.expanded ? 1 : 0.0
                text: deviceAddress
                font.pointSize: 14
                Behavior on opacity {
                    NumberAnimation { duration: 200}
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.expanded = !root.expanded
            root.highlighted = !root.highlighted

            if(root.expanded) {
                deviceSelected(root.delegateIndex);
            } else {
                deviceSelected(-1);
            }
        }

    }
}

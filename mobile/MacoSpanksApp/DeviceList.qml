import QtQuick 2.12
import QtQuick.Layouts 1.12

Item {

    ListView {
        anchors.fill: parent
        model: DeviceListModel {}
        delegate: Rectangle {
            height: listdata.implicitHeight
            width: parent.width
            color: 'red'
            Text {
                id: listdata
                anchors.centerIn: parent
                anchors.fill: parent
                text: name + ' : ' + number
            }
        }
    }

    ListControlBar {
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
    }
}

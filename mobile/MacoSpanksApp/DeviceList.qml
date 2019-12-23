import QtQuick 2.12
import QtQuick.Layouts 1.12

Item {

    ListView {
        anchors.fill: parent
        model: DeviceListModel {}
        delegate: DeviceListDelegate {}
    }

    ListControlBar {
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
    }
}

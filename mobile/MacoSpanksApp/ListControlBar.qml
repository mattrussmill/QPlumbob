import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ToolBar {

        ToolButton {
            id: scanButton
            anchors.left: parent.left
            anchors.fill: parent.height
            width: parent.width / 2
            text: qsTr('Scan')
        }

        ToolButton {
            id: connectButton
            anchors.right: parent.right
            anchors.left: scanButton.right
            anchors.fill: parent.height
            text: qsTr('Connect')
            //swith to disconnect when connected button selected
        }

}

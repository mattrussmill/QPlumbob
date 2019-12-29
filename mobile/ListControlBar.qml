import QtQuick 2.12
import QtQuick.Controls 2.12

ToolBar {
    id: root
    signal scanClicked()
    signal connectClicked()

    ToolButton {
        id: scanButton
        anchors.left: parent.left
        anchors.fill: parent.height
        width: parent.width / 2
        text: qsTr('Scan')
        onClicked: scanClicked()
    }

    ToolButton {
        id: connectButton
        anchors.right: parent.right
        anchors.left: scanButton.right
        anchors.fill: parent.height
        text: qsTr('Connect')
        onClicked: connectClicked()
    }
}

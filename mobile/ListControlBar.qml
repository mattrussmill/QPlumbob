import QtQuick 2.12
import QtQuick.Controls 2.12

ToolBar {
    id: root
    property bool connectEnabled: true
    property bool scanEnabled: true
    signal scanClicked()
    signal connectClicked()

    ToolButton {
        id: scanButton
        anchors.left: parent.left
        anchors.fill: parent.height
        width: parent.width / 2
        text: qsTr('Scan')
        enabled: scanEnabled
        onClicked: scanClicked()
    }

    ToolButton {
        id: connectButton
        anchors.right: parent.right
        anchors.left: scanButton.right
        anchors.fill: parent.height
        text: qsTr('Connect')
        enabled: connectEnabled
        onClicked: connectClicked()
    }
}

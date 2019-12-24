import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ToolBar {
    id: root
    property int scanDuration: 10000

    signal scan(bool scanMode)

    function triggerScan(){
        delayTimer.start()
        root.scan(true)
        root.enabled = false
    }

    function stopScan(){
        delayTimer.stop()
        root.scan(false)
        root.enabled = true
    }

        Timer {
            id: delayTimer
            interval: scanDuration
            onTriggered: root.enabled = true
        }

        ToolButton {
            id: scanButton
            anchors.left: parent.left
            anchors.fill: parent.height
            width: parent.width / 2
            text: qsTr('Scan')

            onClicked: triggerScan()

            onEnabledChanged: {
                if (enabled) //@TODO make state machine for which scan to end
                    root.scan(false)
            }
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

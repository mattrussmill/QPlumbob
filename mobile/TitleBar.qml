import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ToolBar {
    id: root

    signal backButtonClicked()
    signal menuButtonClicked()

    RowLayout {
        anchors.fill: parent

        ToolButton {
            text: qsTr('‹')
            onClicked: {
                root.backButtonClicked()
            }
        }

        Label {
            text: 'MacoSpanks'
            elide: Label.ElideRight
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            Layout.fillWidth: true
        }

        ToolButton {
            text: qsTr('⋮')
            onClicked: {
                console.log('menu clicked')
                root.menuButtonClicked()
            }
        }
    }
}

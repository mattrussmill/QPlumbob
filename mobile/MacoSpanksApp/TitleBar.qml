import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ToolBar {

    RowLayout {
        anchors.fill: parent

        ToolButton {
            text: qsTr('‹')
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
        }
    }
}

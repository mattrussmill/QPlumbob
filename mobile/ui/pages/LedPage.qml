import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Item {
    id: root
    property string attributeName
    readonly property int value: slider.value

    ColumnLayout {
        anchors.fill: parent

        Label {
            text: qsTr(attributeName)
            horizontalAlignment: Qt.AlignHCenter
            Layout.fillWidth: true
        }

        //Image {}
        Rectangle {
            color: 'black'
            Layout.alignment: Qt.AlignCenter
            height: 100
            width: 100

            Rectangle {
                color: 'orange'
                anchors.centerIn: parent
                width: parent.width - 20
                height: 10
            }

        }

        LedSlider {
            id: slider
            Layout.fillWidth: true

        }

    }
}

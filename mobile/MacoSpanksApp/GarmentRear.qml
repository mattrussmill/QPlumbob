import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Item {

    ColumnLayout {
        anchors.fill: parent

        Label {
            text: qsTr('Rear')
            horizontalAlignment: Qt.AlignHCenter
            Layout.fillWidth: true
        }

        //Image {}
        Rectangle {
            color: 'black'
            Layout.alignment: Qt.AlignCenter
            height: 100
            width: 100

        }


        RegionSlider {
            Layout.fillWidth: true
        }


    }
}

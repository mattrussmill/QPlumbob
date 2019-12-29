import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Item {
    id: root
    property string regionName

    ColumnLayout {
        anchors.fill: parent

        Label {
            text: qsTr(regionName)
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

        //create Item for Sliders, also need a status - maybe other side from slider 'success / failed & reset'
        //these will be responsible with communicating with the layer that communicates with the device via properties
        //propogated through
        RegionSlider {
            Layout.fillWidth: true

        }

    }
}

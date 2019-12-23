import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12

Item {

    ColumnLayout {
        anchors.fill: parent

        Label {
            text: qsTr('Front')
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


        //create Item for Sliders, also need a status - maybe other side from slider 'success / failed & reset'
        //these will be responsible with communicating with the layer that communicates with the device via properties
        //propogated through
        Label {
            text: 'Upper'
            horizontalAlignment: Qt.AlignLeft
        }

        Slider {
            Layout.fillWidth: true
        }

        Label {
            text: 'Lower'
            horizontalAlignment: Qt.AlignLeft
        }

        Slider {
            Layout.fillWidth: true
        }

    }
}

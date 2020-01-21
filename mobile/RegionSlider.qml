import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    height: label.height + slider.height

    Label {
        id: label
        text: 'TextLabel'
        anchors.left: parent.left
        anchors.bottom: slider.top
    }

    Slider {
        id: slider
        anchors.bottom: parent.bottom
        width: parent.width
        to: 100
    }

}

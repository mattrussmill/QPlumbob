import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    id: root
    height: label.height + slider.height
    readonly property int value: slider.value

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
        live: false
    }

}

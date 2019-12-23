import QtQuick 2.12

Rectangle {
    height: listdata.implicitHeight
    width: parent.width
    color: 'red'
    Text {
        id: listdata
        anchors.centerIn: parent
        anchors.fill: parent
        text: name + ' : ' + number
    }
}

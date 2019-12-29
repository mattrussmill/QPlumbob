import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    SwipeView {
        id: view
        currentIndex: 0
        anchors.fill: parent

        Garment {
            id: front
            regionName: 'front'
        }

        Garment {
            id: rear
            regionName: 'rear'
        }

    }

    PageIndicator {
        id: indicator
        count: view.count
        currentIndex: view.currentIndex
        anchors.bottom: view.bottom
        anchors.horizontalCenter: parent.horizontalCenter
    }
}

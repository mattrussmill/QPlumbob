import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    SwipeView {
        id: view
        currentIndex: 0
        anchors.fill: parent

        GarmentPage {
            id: frontRegion
            regionName: 'Pelvis'
            onValueChanged: {
                deviceService.setPelvisDutyCycle(value)
            }
        }

        GarmentPage {
            id: rearRegion
            regionName: 'Gluteus'
            onValueChanged: {
                deviceService.setGluteusDutyCycle(value)
            }
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

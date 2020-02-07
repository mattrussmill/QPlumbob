import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Popup {
    id: root
    modal: true

    GridLayout {
        id: layout
        anchors.fill: parent
        columns: 2

        CheckBox {
            id: saveCheckbox

        }

        Label {
            id: saveLabel
            text: "Save Device"
        }


        TextField {
            id: pulseField
            enabled: deviceService.alive
            inputMask: "99"
            implicitWidth: FontMetrics.averageCharacterWidth * 2
            inputMethodHints: Qt.ImhDigitsOnly
            onFocusChanged: {
                if (focus)
                    cursorPosition = text.length
            }
            onCursorPositionChanged: {
                if (text.length == 0)
                    cursorPosition = 0
            }
        }

        Label {
            id: pulseLabel
            enabled: deviceService.alive
            text: "Pulse Duration"
        }


        TextField { // TODO Make component?
            id: safetyField
            enabled: deviceService.alive
            inputMask: "99"
            implicitWidth: FontMetrics.averageCharacterWidth * 2
            inputMethodHints: Qt.ImhDigitsOnly
            onFocusChanged: {
                if (focus)
                    cursorPosition = text.length
            }
            onCursorPositionChanged: {
                if (text.length == 0)
                    cursorPosition = 0
            }
        }

        Label { // TODO Design items with labels for this with a base object
            id: safetyLabel
            enabled: deviceService.alive
            text: "Safety Timeout"
        }
    }
}

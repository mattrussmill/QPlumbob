import QtQuick 2.12

Item {
    id: root

    Rectangle {
        id: canvasContainer
        width: root.width > root.height ? root.height : root.width
        height: width
        anchors.right: root.right
        anchors.bottom: root.bottom
        color: 'transparent'

        // only works in visible part of object
        MouseArea {
            anchors.fill: parent

            //TODO test if want functionality on drag instead of clicked
            onClicked: {
                // length between 2 points - note length and height always the same; in square container
                // sqrt( (x2-x1)^2 + (y2-y1)^2 ) where x2 is fixed where minor arc's 90 deg corner normalized at 100
                var saturation = Math.sqrt(Math.pow(canvasContainer.width - mouse.x, 2)
                                  + Math.pow(canvasContainer.height - mouse.y, 2)) / width * 100

                // arc tangent of (y, x) converted to degrees with the x-axis beginning at x = width
                // angle in radians = arctan (y2 - y1, x2 - x1)
                var angle = Math.atan2(canvasContainer.height - mouse.y, canvasContainer.width - mouse.x)

                //  phase shifted accordingly and normalized at 360 degrees
                var phaseShifted = ((angle + Math.PI / 15) * 180 / Math.PI) / 90 * 360
                var hue = adjusted >= 360 ? adjusted - 360 : adjusted

                console.log('radius: ' + saturation)
                console.log('degrees: ' + hue) // convert from radians?
            }

        }

        Canvas {
            id: canvas
            anchors.fill: parent
            onPaint: {
                var ctx = getContext('2d')
                ctx.reset()

                var xArcCenter = width
                var yArcCenter = height
                var radius = width //always in a square container

                //ConicalGradient - color wheel
                var i = 0.250 / 6
                var gradientColor = ctx.createConicalGradient(xArcCenter, yArcCenter, 0)
                gradientColor.addColorStop(0.250,            Qt.rgba(1, 1, 0, 1))
                gradientColor.addColorStop(0.250 + (i),      Qt.rgba(1, 0, 0, 1))
                gradientColor.addColorStop(0.250 + (i * 2),  Qt.rgba(1, 0, 1, 1))
                gradientColor.addColorStop(0.250 + (i * 3),  Qt.rgba(0, 0, 1, 1))
                gradientColor.addColorStop(0.250 + (i * 4),  Qt.rgba(0, 1, 1, 1))
                gradientColor.addColorStop(0.250 + (i * 5),  Qt.rgba(0, 1, 0, 1))
                gradientColor.addColorStop(0.250 + (i * 6),  Qt.rgba(1, 1, 0, 1))

                ctx.beginPath()
                ctx.fillStyle = gradientColor
                ctx.moveTo(xArcCenter, yArcCenter)
                ctx.arc(xArcCenter, yArcCenter, radius, Math.PI, Math.PI * 3 / 2, false)
                ctx.lineTo(xArcCenter, yArcCenter)
                ctx.fill()

                //RadialGradient - saturation value (LED intensity)
                var gradientSaturation = ctx.createRadialGradient(xArcCenter, yArcCenter, 0, xArcCenter, yArcCenter, radius)
                gradientSaturation.addColorStop(0.15, Qt.rgba(1, 1, 1, 1))
                gradientSaturation.addColorStop(1.00, Qt.rgba(1, 1, 1, 0))

                ctx.beginPath()
                ctx.fillStyle = gradientSaturation
                ctx.moveTo(xArcCenter, yArcCenter)
                ctx.arc(xArcCenter, yArcCenter, radius, Math.PI, Math.PI * 3 / 2, false)
                ctx.lineTo(xArcCenter, yArcCenter)
                ctx.fill()              
            }
        }



    }
}

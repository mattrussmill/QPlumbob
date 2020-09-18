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
            onClicked: {
                //always in square container
                var saturation = Math.sqrt(Math.pow(canvasContainer.width - mouse.x, 2)
                                           + Math.pow(canvasContainer.height - mouse.y, 2))
                                    / width * 100

                console.log('radius: ' + saturation)
                console.log('degrees: ' ) // convert from radians?
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

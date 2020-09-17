import QtQuick 2.12

Item {
    id: root

    Rectangle {
        width: root.width > root.height ? root.height : root.width
        height: width
        anchors.right: root.right
        anchors.bottom: root.bottom
        color: 'transparent'

        Canvas {
            anchors.fill: parent
            onPaint: {
                var ctx = getContext('2d')
                ctx.reset()

                var xArcCenter = width
                var yArcCenter = height
                var radius = width > height ? height : width

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

                //LinearGradient - color value (LED intensity)
                var gradientValue = ctx.createLinearGradient(xArcCenter, yArcCenter, width / 2, height / 2)
                gradientValue.addColorStop(1.0, Qt.rgba(0.65, 0.65, 0.65, 0))
                gradientValue.addColorStop(0.15, Qt.rgba(0.65, 0.65, 0.65, 1))

                ctx.beginPath()
                ctx.fillStyle = gradientValue
                ctx.moveTo(xArcCenter, yArcCenter)
                ctx.arc(xArcCenter, yArcCenter, radius, Math.PI, Math.PI * 3 / 2, false)
                ctx.lineTo(xArcCenter, yArcCenter)
                ctx.fill()

            }
        }



    }
}

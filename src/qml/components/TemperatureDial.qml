import QtQuick
import QtQuick.Controls

Rectangle {
    id: root

    color: "transparent"

    property alias minimumTemperature: dial.minimumTemperature
    property alias maximumTemperature: dial.maximumTemperature
    property alias temperatureCurrent: dial.temperatureCurrent
    property alias temperatureSetpoint: dial.temperatureSetpoint
    property alias stepSize: dial.stepSize

    property bool interacting: false
    signal interactionStarted()
    signal interactionEnded()

    signal temperatureSetpointRequested(real value)

    Item {
        id: dial

        anchors.fill: parent

        property real minimumTemperature: 0
        property real maximumTemperature: 100
        property real temperatureCurrent: 0
        property real temperatureSetpoint: 0
        property real stepSize: 1

        property real value: 0

        // start and end of the dial track
        readonly property real startAngleDeg: 40
        readonly property real endAngleDeg: 140

        // visuals
        readonly property color trackBackgroundColor: "#DDDDDD"
        readonly property color trackFillColorStart: "#2887E0"
        readonly property color trackFillColorMid: "#F3E0FF"
        readonly property color trackFillColorEnd: "#E04324"
        readonly property real trackStrokeWidth: 14
        readonly property color handleColor: "#FFFFFF"
        readonly property color labelColor: "#000000"

        // geometry
        readonly property real radius: Math.min(width, height) / 2
        readonly property real cx: width / 2
        readonly property real cy: height / 2

        // helpers
        function clamp(v, vmin, vmax) { return Math.max(vmin, Math.min(vmax, v)) }
        function rad(d) { return d * Math.PI / 180 }

        // map value -> angle (deg)
        function valueToAngle(v) {
            const angleFloor = startAngleDeg - (startAngleDeg * 2)
            const angleCiel = endAngleDeg + (startAngleDeg * 2)

            let t = (v - minimumTemperature) / (maximumTemperature - minimumTemperature)
            t = 1 - t

            let a = angleFloor + t * (angleCiel - angleFloor)
            a *= -1

            // console.log(`valueToAngle: t=${t},a=${a}`)
            return a
        }

        // map angle (deg) -> value
        function angleToValue(angleDeg) {
            const angleFloor = startAngleDeg - (startAngleDeg * 2)
            const angleCiel = endAngleDeg + (startAngleDeg * 2)

            const angleDegClamped = clamp(angleDeg, angleFloor, angleCiel)

            const t = (angleDegClamped - angleFloor) / (angleCiel - angleFloor)
            const v = minimumTemperature + t * (maximumTemperature - minimumTemperature)

            // console.log(`angleToValue: angleDeg=${angleDeg},angleDegClamped=${angleDegClamped},t=${t},v=${v}`)
            return v
        }

        // map angle (deg) -> point on the track
        function angleToPoint(angleDeg, w, h) {
            const r = radius - trackStrokeWidth
            const angleRad = rad(angleDeg)

            const px = cx + Math.cos(angleRad) * r - w / 2
            const py = cy + Math.sin(angleRad) * r - h / 2

            // console.log(`onAngleDegChanged: angleDeg=${angleDeg},angleRad=${angleRad},r=${r},x=${px},y=${py}`)
            return { x: px, y: py }
        }

        // angle from center in 360 deg
        function angleFromPoint(x, y) {
            const dx = x - cx
            const dy = y - cy

            let a = Math.atan2(dy, dx) * (180 / Math.PI)
            a = (a + 180) % 360
            if (a > 360 - startAngleDeg) {
                a -= 360
            }

            // console.log(`angleFromPoint: x=${x},y=${y},dx=${dx},dy=${dy},a=${a}`)
            return a
        }

        // check if point is on the track
        function isPointOnArc(x, y) {
            const r = radius - trackStrokeWidth
            const dx = x - cx
            const dy = y - cy

            const dist = Math.sqrt(dx*dx + dy*dy)
            const v = dist >= r - trackStrokeWidth && dist <= r + trackStrokeWidth

            // console.log(`isPointOnArc: x=${x},y=${y},dx=${dx},dy=${dy},dist=${dist},r=${r},v=${v}`)
            return v
        }

        // handle user input to temperature
        function updateValue(v) {
            if (v === value) return

            if (!root.interacting) {
                root.interacting = true
                root.interactionStarted()
            }

            v = clamp(v, minimumTemperature, maximumTemperature)

            if (!temperatureUpdateTimer.running) {
                temperatureUpdateTimer.temperatureCurrentRestore = temperatureCurrent
            }
            temperatureUpdateTimer.restart()

            value = v
        }

        // track
        Canvas {
            id: canvas
            anchors.fill: parent
            onPaint: {
                const ctx = getContext("2d")

                ctx.clearRect(0, 0, width, height)

                const r = Math.min(width, height) / 2 - dial.trackStrokeWidth
                const cx = width / 2
                const cy = height / 2

                // track background
                ctx.beginPath()
                ctx.lineWidth = dial.trackStrokeWidth
                ctx.lineCap = "round"
                ctx.strokeStyle = dial.trackBackgroundColor
                ctx.arc(cx, cy, r, dial.rad(dial.startAngleDeg), dial.rad(dial.endAngleDeg), true)
                ctx.stroke()
                ctx.closePath()

                // track fill
                if (dial.value === minimumTemperature) return

                let gradient = ctx.createLinearGradient(0, cy, width, cy)
                gradient.addColorStop(0, dial.trackFillColorStart)
                gradient.addColorStop(0.5, dial.trackFillColorMid)
                gradient.addColorStop(1, dial.trackFillColorEnd)

                const currentAngle = dial.valueToAngle(dial.value)

                ctx.beginPath()
                ctx.lineWidth = dial.trackStrokeWidth
                ctx.lineCap = "round"
                ctx.strokeStyle = gradient
                ctx.arc(cx, cy, r, dial.rad(dial.endAngleDeg), dial.rad(currentAngle), false)
                ctx.stroke()
                ctx.closePath()
            }
        }

        // handle
        Rectangle {
            id: handle

            width: dial.trackStrokeWidth + 3
            height: dial.trackStrokeWidth + 3
            radius: width / 2
            z: temperatureSetpointIndicator.z + 1

            color: dial.handleColor
            border.color: "#888888"
            border.width: 1.5
            antialiasing: true

            property real angleDeg: dial.valueToAngle(dial.value)

            onAngleDegChanged: {
                const p = dial.angleToPoint(angleDeg, width, height)
                x = p.x
                y = p.y
            }

            Component.onCompleted: angleDeg = dial.valueToAngle(dial.value)
        }

        // setpoint indicator
        Rectangle {
            id: temperatureSetpointIndicator

            width: dial.trackStrokeWidth / 2
            height: dial.trackStrokeWidth / 2
            radius: width / 2
            z: 2

            color: "#000000"
            opacity: 0.8
            antialiasing: true

            property real angleDeg: dial.valueToAngle(dial.temperatureSetpoint)

            onAngleDegChanged: {
                const p = dial.angleToPoint(angleDeg, width, height)
                x = p.x
                y = p.y
            }

            Component.onCompleted: angleDeg = dial.valueToAngle(dial.temperatureSetpoint)
        }

        Label {
            id: temperatureCurrentLabel

            parent: mouseArea

            x: (dial.width / 2) - (width / 2)
            y: (dial.height / 2) - (height / 2) - 16

            color: dial.labelColor
            text: Math.round(dial.temperatureCurrent) + "℉"

            font.pixelSize: 32
        }
        Label {
            id: temperatureSetpointLabel

            parent: mouseArea

            anchors.top: temperatureCurrentLabel.bottom
            anchors.topMargin: 5
            x: temperatureCurrentLabel.x + (temperatureCurrentLabel.width / 2) - (width / 2)

            color: dial.labelColor
            text: Math.round(dial.temperatureSetpoint) + "℉"

            font.pixelSize: 16
        }

        Button {
            id: decrementButton

            width: 22
            height: 22

            x: {
                const r = dial.radius - dial.trackStrokeWidth
                const angleRad = dial.rad(360 - dial.startAngleDeg)
                const v = dial.cy + Math.sin(angleRad) * r - width / 2
                return v + width
            }
            y: {
                const r = dial.radius - dial.trackStrokeWidth
                const angleRad = dial.rad(360 - dial.startAngleDeg)
                const v = dial.cx + Math.cos(angleRad) * r - height / 2
                return v
            }

            text: "-"

            onReleased: {
                dial.updateValue(dial.value - 1)
            }
        }
        Button {
            id: incrementButton

            width: 22
            height: 22

            anchors.top: decrementButton.top
            x: dial.width - decrementButton.x - width

            text: "+"

            onReleased: {
                dial.updateValue(dial.value + 1)
            }
        }

        MouseArea {
            id: mouseArea

            anchors.top: parent.top
            anchors.bottom: decrementButton.top
            anchors.left: parent.left
            width: parent.width

            hoverEnabled: true

            property bool dragging: false

            onPressed: function(mouse) {
                const isMouseOnTrackOrHandle = dial.isPointOnArc(mouse.x, mouse.y)
                if (!isMouseOnTrackOrHandle) return

                dragging = true
                mouse.accepted = true

                const a = dial.angleFromPoint(mouse.x, mouse.y)
                let newValue = dial.angleToValue(a)

                // console.log(`onPressed: x=${mouse.x},y=${mouse.y},a=${a},newVal=${newVal},isMouseOnTrackOrHandle=${isMouseOnTrackOrHandle}`)

                const step = dial.stepSize
                if (step > 0) newValue = Math.round(newValue / step) * step
                dial.updateValue(newValue)
            }
            onPositionChanged: function(mouse) {
                if (!dragging) return

                const a = dial.angleFromPoint(mouse.x, mouse.y)
                if (a > dial.endAngleDeg + (dial.startAngleDeg * 2)) return
                let newValue = dial.angleToValue(a)

                const step = dial.stepSize
                if (step > 0) newValue = Math.round(newValue / step) * step
                dial.updateValue(newValue)
            }
            onReleased: {
                dragging = false
            }
        }

        WheelHandler {
            id: wheelHandler
            onWheel: function(wheel) {
                const delta = wheel.angleDelta.y !== undefined ? wheel.angleDelta.y : wheel.delta.y
                let change = (delta / 120) * dial.stepSize
                if (change === 0) change = (delta > 0 ? dial.stepSize : -dial.stepSize)
                const newValue = dial.clamp(Math.round((dial.value + change) / dial.stepSize) * dial.stepSize, dial.minimumTemperature, dial.maximumTemperature)
                dial.updateValue(newValue)
            }
        }

        PropertyAnimation {
            id: temperatureCurrentAnimation
            target: dial
            property: "temperatureCurrent"
            to: 0
            duration: 1500
            easing.type: Easing.InOutQuart
        }
        PropertyAnimation {
            id: temperatureSetpointAnimation
            target: dial
            property: "temperatureSetpoint"
            to: 0
            duration: 1500
            easing.type: Easing.InOutQuart
        }
        PropertyAnimation {
            id: temperatureSetpointLabelAnimation
            target: temperatureSetpointLabel
            property: "opacity"
            to: 0
            duration: 1500
        }

        Timer {
            id: temperatureUpdateTimer
            interval: 2000
            running: false
            repeat: false

            property real temperatureCurrentRestore: 0
            property bool working: false

            onTriggered: {
                if (mouseArea.dragging) {
                    restart()
                    return
                }

                if (dial.temperatureCurrent === temperatureCurrentRestore) {
                    root.interacting = false
                    root.interactionEnded()
                    return
                }

                working = true

                if (dial.temperatureSetpoint !== dial.temperatureCurrent) {
                    root.temperatureSetpointRequested(dial.temperatureSetpoint)

                    if (temperatureSetpointLabel.opacity === 0) {
                        dial.temperatureSetpoint = dial.temperatureCurrent
                        temperatureSetpointLabel.opacity = 100
                        temperatureSetpointIndicator.visible = true
                    } else {
                        temperatureSetpointAnimation.to = dial.temperatureCurrent
                        temperatureSetpointAnimation.start()
                    }
                }

                temperatureCurrentAnimation.to = temperatureCurrentRestore
                temperatureCurrentAnimation.start()

                working = false

                root.interacting = false
                root.interactionEnded()
            }
        }

        function checkSetpointEqualsCurrent() {
            if (
                    !temperatureUpdateTimer.running
                    && !temperatureUpdateTimer.working
                    && !temperatureSetpointAnimation.running
                    && !temperatureCurrentAnimation.running
                    && temperatureCurrent === temperatureSetpoint
            ) {
                temperatureSetpointLabelAnimation.start()
                temperatureSetpointIndicator.visible = false
            }
        }

        onTemperatureSetpointChanged: {
            checkSetpointEqualsCurrent()
            temperatureSetpointIndicator.angleDeg = valueToAngle(temperatureSetpoint)
        }
        onTemperatureCurrentChanged: {
            checkSetpointEqualsCurrent()
            value = temperatureCurrent
        }
        onValueChanged: {
            temperatureCurrent = value
            handle.angleDeg = valueToAngle(value)
            canvas.requestPaint()
        }

        Component.onCompleted: {
            value = temperatureCurrent
            canvas.requestPaint()
        }
    }
}

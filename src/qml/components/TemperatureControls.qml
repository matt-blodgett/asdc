import QtQuick
import QtQuick.Controls

Item {
    id: root

    Column {
        anchors.centerIn: parent
        spacing: 8

        Row {
            spacing: 8
            anchors.horizontalCenter: parent.horizontalCenter

            Label {
                id: temperatureLabel
                text: core.messageLive.temperatureFahrenheit
                font.pixelSize: 32
                horizontalAlignment: Text.AlignHCenter
                // width: parent.width
            }
            Label {
                id: setpointLabel
                text: core.messageLive.temperatureSetpointFahrenheit
                font.pixelSize: 32
                horizontalAlignment: Text.AlignHCenter
                // width: parent.width
            }
        }

        Row {
            spacing: 8
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                id: downButton
                text: "-"
                onReleased: temperatureSetpointSlider.decrease()
            }

            Slider {
                id: temperatureSetpointSlider
                from: core.messageSettings.minTemperature
                to: core.messageSettings.maxTemperature
                value: core.messageLive.temperatureSetpointFahrenheit
                stepSize: 1
                width: 200
                onValueChanged: {
                    // console.log(`pressed=${pressed}, value=${value}`)
                    if (!pressed && value !== core.messageLive.temperatureSetpointFahrenheit) {
                        core.commandSetTemperatureSetpointFahrenheit(value)
                    }
                }
                onPressedChanged: {
                    // console.log(`pressed=${pressed}, value=${value}`)
                    if (!pressed && value !== core.messageLive.temperatureSetpointFahrenheit) {
                        core.commandSetTemperatureSetpointFahrenheit(value)
                    }
                }
            }

            Button {
                id: upButton
                text: "+"
                onReleased: temperatureSetpointSlider.increase()
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.color: "#00FF00"
        border.width: 1
        radius: 2
        z: -1
    }
}

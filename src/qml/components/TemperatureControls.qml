import QtQuick
import QtQuick.Controls

Item {
    id: root

    property bool showSetpoint : false

    Column {
        anchors.centerIn: parent
        spacing: 4

        Row {
            spacing: 12
            anchors.horizontalCenter: parent.horizontalCenter

            Label {
                id: temperatureTypeLabel
                anchors.verticalCenter: parent.verticalCenter
                text: showSetpoint ? "Setpoint" : "Current"
                font.pixelSize: 14
            }
            Label {
                id: temperatureValueLabel
                anchors.verticalCenter: parent.verticalCenter
                text: showSetpoint ? temperatureSetpointSlider.value : core.messageLive.temperatureFahrenheit
                font.pixelSize: 32
            }
        }

        Row {
            spacing: 8
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                id: decreaseTemperatureSetpointButton
                text: "-"
                onReleased: temperatureSetpointSlider.decrease()
            }

            Slider {
                id: temperatureSetpointSlider

                width: 200

                from: core.messageSettings.minTemperature
                to: core.messageSettings.maxTemperature
                stepSize: 1
                value: 0

                function setTemperatureSetpoint() {
                    if (isNaN(core.messageLiveReceivedAt.getTime())) {
                        return
                    }
                    const newValue = Number(value)
                    const currentValue = Number(core.messageLive.temperatureSetpointFahrenheit)
                    // console.log(`pressed=${pressed}, value=${newValue}, current=${currentValue}, changed=${newValue !== currentValue}`)
                    if (!pressed && newValue !== currentValue) {
                        core.commandSetTemperatureSetpointFahrenheit(value)
                    }
                }

                onValueChanged: setTemperatureSetpoint()
                onPressedChanged: {
                    setTemperatureSetpoint()
                    showSetpoint = pressed
                }
            }

            Button {
                id: increaseTemperatureSetpointButton
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

    Component.onCompleted: {

    }

    Connections {
        target: core
        // enabled: root.visible
        function onMessageLiveChanged() {
            console.log("setting slider value")
            temperatureSetpointSlider.value = core.messageLive.temperatureSetpointFahrenheit
        }
    }
}

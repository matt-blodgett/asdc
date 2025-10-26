import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property bool controlsEnabled: true
    property bool showingSetpoint : false

    function setShowingSetpoint(value) {
        if (value) {
            hideSetpointTimer.restart()
        } else {
            temperatureSlider.value = core.messageLive.temperatureFahrenheit
        }
        showingSetpoint = value
    }

    Timer {
        id: hideSetpointTimer
        interval: 3000
        running: false
        repeat: false
        onTriggered: {
            if (temperatureSlider.pressed) {
                restart()
                return
            }
            setShowingSetpoint(false)
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 4

        Item {
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 1
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.horizontalStretchFactor: 1
            Layout.verticalStretchFactor: 1

            // color: "#FF0000"

            RowLayout {
                id: temperatureInfoLayout
                anchors.centerIn: parent
                spacing: 12

                Label {
                    id: temperatureTypeLabel
                    text: showingSetpoint ? "Setpoint" : "Current"
                    font.pixelSize: 14
                }
                Label {
                    id: temperatureValueLabel
                    text: showingSetpoint ? temperatureSlider.value : core.messageLive.temperatureFahrenheit
                    font.pixelSize: 28
                }
            }
            MouseArea {
                anchors.fill: temperatureInfoLayout
                hoverEnabled: true
                onEntered: setShowingSetpoint(true)
                onExited: setShowingSetpoint(false)
            }
        }

        RowLayout {
            Layout.topMargin: 10

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.horizontalStretchFactor: 1
            Layout.verticalStretchFactor: 1

            spacing: 6

            Button {
                implicitWidth: 20
                implicitHeight: 20
                text: "-"
                enabled: controlsEnabled
                onReleased: temperatureSlider.decreaseTemperatureSetpoint()
            }
            Slider {
                id: temperatureSlider

                Layout.fillWidth: true
                // Layout.maximumWidth: 300
                // width: 200

                from: core.messageSettings.minTemperature
                to: core.messageSettings.maxTemperature
                stepSize: 1
                value: 0

                enabled: controlsEnabled

                function applyTemperatureSetpoint() {
                    if (isNaN(core.messageLiveReceivedAt.getTime())) {
                        return
                    }

                    setShowingSetpoint(true)

                    const newValue = Number(value)
                    const currentValue = Number(core.messageLive.temperatureSetpointFahrenheit)
                    if (!pressed && newValue !== currentValue) {
                        // controlsEnabled = false
                        core.commandSetTemperatureSetpointFahrenheit(value)
                    }
                }
                function stepTemperatureSetpoint(func) {
                    value = core.messageLive.temperatureSetpointFahrenheit
                    func()
                    applyTemperatureSetpoint()
                }
                function increaseTemperatureSetpoint() {
                    stepTemperatureSetpoint(increase)
                }
                function decreaseTemperatureSetpoint() {
                    stepTemperatureSetpoint(decrease)
                }

                onPressedChanged: {
                    if (pressed) {
                        setShowingSetpoint(true)
                    } else {
                        applyTemperatureSetpoint()
                    }
                }
            }
            Button {
                implicitWidth: 20
                implicitHeight: 20
                text: "+"
                enabled: controlsEnabled
                onReleased: temperatureSlider.increaseTemperatureSetpoint()
            }
        }
        Item {
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 1
        }
    }

    // Component.onCompleted: {
    // }

    Connections {
        target: core
        // enabled: root.visible
        function onMessageLiveChanged() {
            controlsEnabled = true
            if (!showingSetpoint) {
                temperatureSlider.value = core.messageLive.temperatureFahrenheit
            }
        }
    }

    // Rectangle {
    //     anchors.fill: parent
    //     color: "transparent"
    //     border.color: "#00FF00"
    //     border.width: 1
    //     radius: 2
    //     z: -1
    // }
}

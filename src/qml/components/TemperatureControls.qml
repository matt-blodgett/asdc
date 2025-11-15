import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    TemperatureDial {
        id: temperatureDial

        width: 180
        height: 180

        border.color: "#FF0000"
        border.width: 1

        // enabled: false

        minimumTemperature: core.messageSettings.minTemperature
        maximumTemperature: core.messageSettings.maxTemperature
        temperatureCurrent: 0
        temperatureSetpoint: 0
        stepSize: 1

        onTemperatureSetpointRequested: function(value) {
            console.log(`onRequestedTemperatureSetpoint: ${value}`)
        }
    }

    // Component.onCompleted: {
    // }

    Connections {
        target: core
        // enabled: root.visible
        function onMessageLiveChanged() {
            // controlsEnabled = true

            if (!temperatureDial.interacting) {
                temperatureDial.temperatureCurrent = core.messageLive.temperatureFahrenheit
                temperatureDial.temperatureSetpoint = core.messageLive.temperatureSetpointFahrenheit
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

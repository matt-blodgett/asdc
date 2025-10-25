import QtQuick
// import QtQuick.Controls.Basic
import QtQuick.Controls
import QtQuick.Layouts

import asdc.pb

Item {
    id: root

    property bool controlsEnabled: true

    function pumpStatusValue(pumpNumber) {
        return core.messageLive[`pump${pumpNumber}`]
    }
    function pumpStatusText(pumpNumber) {
        const pumpStatus = pumpStatusValue(pumpNumber)
        if (pumpStatus === Live.PUMP_HIGH) {
            return "HIGH"
        }
        if (pumpStatus === Live.PUMP_LOW) {
            return "LOW"
        }
        return "OFF"
    }
    function toggleControl(controlName) {
        const currentValue = core.messageLive[controlName]
        // const nextValue = !currentValue

        let nextValue = null

        if (typeof currentValue === "boolean") {
            nextValue = !currentValue
        } else if (controlName === "pump1") {
            if (currentValue === Live.PUMP_OFF) {
                nextValue = Live.PUMP_LOW
            } else if (currentValue === Live.PUMP_LOW) {
                nextValue = Live.PUMP_HIGH
            } else if (currentValue === Live.PUMP_HIGH) {
                nextValue = Live.PUMP_OFF
            }
        } else if (controlName.startsWith("pump")) {
            nextValue = currentValue === Live.PUMP_OFF ? Live.PUMP_HIGH : Live.PUMP_OFF
        }

        if (nextValue === null) {
            return
        }

        const commandName = controlName.charAt(0).toUpperCase() + controlName.slice(1)

        controlsEnabled = false
        core[`commandSet${commandName}`](nextValue)
    }

    Connections {
        target: core
        // enabled: root.visible
        function onMessageLiveChanged() {
            controlsEnabled = true
        }
    }

    GridLayout {
        anchors.fill: parent
        columns: 3
        rowSpacing: 5
        columnSpacing: 5

        Button {
            Layout.row: 0
            Layout.column: 0
            implicitWidth: 100
            text: "Pump 1 - " + pumpStatusText(1)
            enabled: controlsEnabled
            onReleased: toggleControl("pump1")
        }
        Button {
            Layout.row: 0
            Layout.column: 1
            text: "Pump 2 - " + pumpStatusText(2)
            enabled: controlsEnabled
            onReleased: toggleControl("pump2")
        }
        Button {
            Layout.row: 0
            Layout.column: 2
            text: "Pump 3 - " +  pumpStatusText(3)
            enabled: controlsEnabled
            onReleased: toggleControl("pump3")
        }
        Button {
            Layout.row: 1
            Layout.column: 0
            text: "Lights - " + (core.messageLive.lights ? "ON" : "OFF")
            enabled: controlsEnabled
            onReleased: toggleControl("lights")
        }
        Button {
            Layout.row: 1
            Layout.column: 1
            text: "EZ - " + (core.messageLive.allOn ? "ON" : "OFF")
            enabled: controlsEnabled
            onReleased: toggleControl("allOn")
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.color: "#0000FF"
        border.width: 1
        radius: 2
        z: -1
    }
}




// Image {
//     id: testImage
//     Layout.row: 0
//     Layout.column: 0
//     Layout.maximumWidth: 60
//     Layout.maximumHeight: 60

//     width: 60
//     height: 60
//     source: "qrc:/assets/icons/controls/pump1_off.svg"
//     MouseArea {
//         anchors.fill: parent
//         hoverEnabled: true
//         onEntered: {
//             if (deviceClient.pump2 === 2) {
//                 testImage.source = "qrc:/assets/icons/controls/pump1_on_hover.svg"
//             } else {
//                 testImage.source = "qrc:/assets/icons/controls/pump1_off_hover.svg"
//             }
//         }
//         onExited: {
//             if (deviceClient.pump2 === 2) {
//                 testImage.source = "qrc:/assets/icons/controls/pump1_on.svg"
//             } else {
//                 testImage.source = "qrc:/assets/icons/controls/pump1_off.svg"
//             }
//         }
//         onReleased: {
//             // testImage.source = "qrc:/assets/icons/controls/pump1_on.svg"
//         }
//         // onPressAndHold: {
//         //     testImage.source = "qrc:/assets/icons/check_circle.svg"
//         // }
//         onReleased: {
//             testImage.source = "qrc:/assets/icons/controls/pump1_on.svg"
//             let state = 2
//             if (deviceClient.pump2 === 2) {
//                 state = 0
//             }
//             deviceClient.setPump2(state)
//         }
//     }
// }

// Connections {
//     target: deviceClient
//     function onPump2Changed () {
//         console.log("frontend got backend pump2 changed -> " + deviceClient.pump2)
//         if (deviceClient.pump2 === 2) {
//             testImage.source = "qrc:/assets/icons/controls/pump1_on.svg"
//         } else {
//             testImage.source = "qrc:/assets/icons/controls/pump1_off.svg"
//         }
//     }
// }


// Button {
//     Layout.row: 0
//     Layout.column: 0
//     flat: true
//     icon.source: {
//         if (down) {
//             return "qrc:/assets/icons/check_circle.svg"
//         }
//         if (hovered) {
//             return "qrc:/assets/icons/refresh.svg"
//         }
//         return "qrc:/assets/icons/error_circle.svg"
//     }
//     onReleased: {
//         console.log("Pump 1 -> " + deviceClient.pump1)
//     }
// }

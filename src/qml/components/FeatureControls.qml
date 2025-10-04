import QtQuick
// import QtQuick.Controls.Basic
import QtQuick.Controls
import QtQuick.Layouts

import asdc.pb

Item {
    id: root

    function pumpStatusText(status) {
        if (status === Live.PUMP_HIGH) {
            return "HIGH"
        }
        if (status === Live.PUMP_LOW) {
            return "LOW"
        }
        return "OFF"
    }

    GridLayout {
        anchors.fill: parent
        columns: 3
        rowSpacing: 5
        columnSpacing: 5

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

        Button {
            Layout.row: 0
            Layout.column: 0
            implicitWidth: 100
            text: "Pump 1 - " + pumpStatusText(core.messageLive.pump1)
            onReleased: {
                console.log(`pump1 -> ${core.messageLive.pump1}`)
            }
        }
        Button {
            Layout.row: 0
            Layout.column: 1
            text: "Pump 2 - " + pumpStatusText(core.messageLive.pump2)
            onReleased: {
                console.log(`pump2 -> ${core.messageLive.pump2}`)
            }
        }
        Button {
            Layout.row: 0
            Layout.column: 2
            text: "Pump 3 - " +  pumpStatusText(core.messageLive.pump3)
            onReleased: {
                console.log(`pump3 -> ${core.messageLive.pump3}`)
            }
        }
        Button {
            Layout.row: 1
            Layout.column: 0
            text: "Lights - " + (core.messageLive.lights ? "ON" : "OFF")
            onReleased: {
                console.log(`lights -> ${core.messageLive.lights}`)
            }
        }
        Button {
            Layout.row: 1
            Layout.column: 1
            text: "EZ"
            onReleased: {
                console.log("EZ clicked")
            }
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

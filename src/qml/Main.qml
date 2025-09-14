import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

import asdc.types.sockets


ApplicationWindow {
    id: mainWindow
    title: qsTr("Hello World")
    visible: true
    minimumWidth: 400
    minimumHeight: 300

    Settings {
        id: uiSettings
        category: "mainWindow"
        property alias x: mainWindow.x
        property alias y: mainWindow.y
        property alias width: mainWindow.width
        property alias height: mainWindow.height
    }

    Settings {
        id: clientSettings
        category: "client"
        property alias hostIpAddress: hostIpAddressInput.text
    }

    menuBar: MenuBar {
        Menu {
            title: "File"
            MenuItem {
                text: "Test"
                onTriggered: {
                    console.log("test")
                }
            }
        }
        MenuSeparator {}
        MenuItem {
            text: "&Open"
            onTriggered: console.log("Open action triggered")
        }
        MenuSeparator {}
        MenuItem {
            text: "Exit"
            onTriggered: Qt.quit()
        }
    }

    Item {
        anchors.fill: parent
        anchors.topMargin: 5
        anchors.bottomMargin: 5
        anchors.leftMargin: 10
        anchors.rightMargin: 10

        RowLayout {
            anchors.fill: parent
            spacing: 0

            Rectangle {
                id: connectionContainer

                Layout.fillWidth: true
                Layout.horizontalStretchFactor: 0
                Layout.fillHeight: true

                implicitWidth: hostIpAddressInput.implicitWidth + connectButton.implicitWidth + connectionStatusLabel.implicitWidth + 40;

                color: "#AA0000"

                Column {
                    spacing: 10

                    Row {
                        spacing: 10

                        TextField {
                            id: hostIpAddressInput
                            text: ""
                            // readOnly: true
                        }
                        Button {
                            id: connectButton
                            text: {
                                if (core.clientState === SocketState.UnconnectedState) {
                                    return "Connect"
                                } else if (core.clientState === SocketState.ConnectedState) {
                                    return "Disconnect"
                                }
                                return "Loading..."
                            }
                            enabled: {
                                if (core.clientState === SocketState.UnconnectedState && hostIpAddressInput.text) {
                                    return true
                                }
                                return core.clientState === SocketState.ConnectedState
                            }
                            onReleased: {
                                if (core.clientState === SocketState.UnconnectedState) {
                                    const host = hostIpAddressInput.text
                                    core.connectClient(host)
                                } else if (core.clientState === SocketState.ConnectedState) {
                                    core.disconnectClient()
                                }
                            }
                        }
                        Label {
                            id: connectionStatusLabel
                            text: {
                                if (core.clientState === SocketState.UnconnectedState) {
                                    return "Disconnected"
                                } else if (core.clientState === SocketState.HostLookupState || core.clientState === SocketState.ConnectingState) {
                                    return "Connecting..."
                                } else if (core.clientState === SocketState.ConnectedState) {
                                    return "Connected"
                                } else if (core.clientState === SocketState.ClosingState) {
                                    return "Disconnecting..."
                                }
                                return "Unknown"
                            }
                        }
                    }

                    Row {
                        spacing: 10

                        Button {
                            id: refreshButton
                            text: "Refresh"
                            onReleased: {
                                core.refreshMessageLive()
                            }
                        }
                    }
                }
            }

            Rectangle {
                id: tableMessageContainer

                Layout.fillWidth: true
                Layout.horizontalStretchFactor: 1
                Layout.fillHeight: true

                color: "transparent"

                RowLayout {
                    anchors.fill: parent
                    spacing: 0

                    Rectangle {
                        Layout.topMargin: 1
                        Layout.bottomMargin: 1
                        Layout.leftMargin: 1
                        Layout.rightMargin: 1

                        Layout.fillWidth: true
                        Layout.horizontalStretchFactor: 1
                        Layout.fillHeight: true
                        Layout.verticalStretchFactor: 1

                        color: "#FFFFFF"
                        border.color: "#000000"
                        border.width: 1

                        HorizontalHeaderView {
                            id: messageTableHeader

                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.right: parent.right

                            clip: true

                            model: ["Field", "Value"]
                            syncView: messageTableView
                        }

                        TableView {
                            id: messageTableView

                            anchors.left: parent.left
                            anchors.top: messageTableHeader.bottom
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom

                            clip: true

                            rowSpacing: 0
                            columnSpacing: 0

                            resizableColumns: true

                            editTriggers: TableView.NoEditTriggers
                            boundsBehavior: Flickable.StopAtBounds

                            columnWidthProvider: function(column) {
                                // console.log(`column=${column},isColumnLoaded=${isColumnLoaded(column)}`)
                                if (!isColumnLoaded(column)) {
                                    return -1
                                }

                                const lastColumnIndex = columns - 1
                                if (column === lastColumnIndex) {
                                    const tableWidth = messageTableView.width

                                    let totalColumnWidths = 0
                                    for (let i = 0; i < columns; i++) {
                                        totalColumnWidths += columnWidth(i)
                                    }

                                    const widthDifference = tableWidth - totalColumnWidths

                                    const lastColumnWidth = columnWidth(lastColumnIndex)
                                    const lastColumnImplicitWidth = implicitColumnWidth(lastColumnIndex)

                                    const lastColumnWidthAdjusted = Math.max(lastColumnWidth + widthDifference, lastColumnImplicitWidth)

                                    // console.log(`lastColumnIndex          =${lastColumnIndex}`)
                                    // console.log(`tableWidth               =${tableWidth}`)
                                    // console.log(`totalColumnWidths        =${totalColumnWidths}`)
                                    // console.log(`widthDifference          =${widthDifference}`)
                                    // console.log(`lastColumnWidth          =${lastColumnWidth}`)
                                    // console.log(`lastColumnImplicitWidth  =${lastColumnImplicitWidth}`)
                                    // console.log(`lastColumnWidthAdjusted  =${lastColumnWidthAdjusted}`)
                                    // console.log('-----------------------------------')

                                    return lastColumnWidthAdjusted
                                }

                                let w = explicitColumnWidth(column)
                                if (w >= 0) {
                                    return Math.max(100, w)
                                }
                                return implicitColumnWidth(column)
                            }
                            // Timer {
                            //     running: true
                            //     interval: 2000
                            //     onTriggered: {
                            //         messageTableView.forceLayout();
                            //     }
                            // }

                            delegate: TableViewDelegate {
                                leftPadding: 5
                                rightPadding: 5
                                topPadding: 5
                                bottomPadding: 5
                            }
                            model: TableModel {
                                TableModelColumn { display: "field" }
                                TableModelColumn { display: "value" }
                                rows: []
                            }

                            readonly property list<string> fieldsClock : [
                                "year",
                                "month",
                                "day",
                                "hour",
                                "minute",
                                "second"
                            ]
                            readonly property list<string> fieldsConfiguration : [
                                "pump1",
                                "pump2",
                                "pump3",
                                "pump4",
                                "pump5",
                                "blower1",
                                "blower2",
                                "lights",
                                "stereo",
                                "heater1",
                                "heater2",
                                "filter",
                                "onzen",
                                "ozonePeak1",
                                "ozonePeak2",
                                "exhaustFan",
                                "powerlines",
                                "breakerSize",
                                "smartOnzen",
                                "fogger",
                                "sds",
                                "yess"
                            ]
                            readonly property list<string> fieldsError : [
                                "noFlow",
                                "flowSwitch",
                                "heaterOverTemperature",
                                "spaOverTemperature",
                                "spaTemperatureProbe",
                                "spaHighLimit",
                                "eeprom",
                                "freezeProtect",
                                "phHigh",
                                "heaterProbeDisconnected"
                            ]
                            readonly property list<string> fieldsFilter : [
                                "serialNums",
                                "filterState",
                                "installDates"
                            ]
                            readonly property list<string> fieldsInformation : [
                                "packSerialNumber",
                                "packFirmwareVersion",
                                "packHardwareVersion",
                                "packProductId",
                                "packBoardId",
                                "topsideProductId",
                                "topsideSoftwareVersion",
                                "guid",
                                "spaType",
                                "websiteRegistration",
                                "websiteRegistrationConfirm",
                                "macAddress",
                                "firmwareVersion",
                                "productCode",
                                "varSoftwareVersion",
                                "spaboyFirmwareVersion",
                                "spaboyHardwareVersion",
                                "spaboyProductId",
                                "spaboySerialNumber",
                                "rfidFirmwareVersion",
                                "rfidHardwareVersion",
                                "rfidProductId",
                                "rfidSerialNumber"
                            ]
                            readonly property list<string> fieldsLive : [
                                "temperatureFahrenheit",
                                "temperatureSetpointFahrenheit",
                                "pump1",
                                "pump2",
                                "pump3",
                                "pump4",
                                "pump5",
                                "blower1",
                                "blower2",
                                "lights",
                                "stereo",
                                "heater1",
                                "heater2",
                                "filter",
                                "onzen",
                                "ozone",
                                "exhaustFan",
                                "sauna",
                                "heaterAdc",
                                "saunaTimeRemaining",
                                "economy",
                                "currentAdc",
                                "allOn",
                                "fogger",
                                "error",
                                "alarm",
                                "status",
                                "ph",
                                "orp",
                                "sds",
                                "yess"
                            ]
                            readonly property list<string> fieldsOnzenLive : [
                                "guid",
                                "orp",
                                "ph100",
                                "current",
                                "voltage",
                                "currentSetpoint",
                                "voltageSetpoint",
                                "pump1",
                                "pump2",
                                "orpStateMachine",
                                "electrodeStateMachine",
                                "electrodeId",
                                "electrodePolarity",
                                "electrode1Resistance1",
                                "electrode1Resistance2",
                                "electrode2Resistance1",
                                "electrode2Resistance2",
                                "commandMode",
                                "electrodeMAH",
                                "phColor",
                                "orpColor",
                                "electrodeWear"
                            ]
                            readonly property list<string> fieldsOnzenSettings : [
                                "guid",
                                "overVoltage",
                                "underVoltage",
                                "overCurrent",
                                "underCurrent",
                                "orpHigh",
                                "orpLow",
                                "phHigh",
                                "phLow",
                                "pwmPump1TimeOn",
                                "pwmPump1TimeOff",
                                "samplingInterval",
                                "samplingDuration",
                                "pwmPump2TimeOn",
                                "pwmPump2TimeOff",
                                "sbLowCl",
                                "sbCautionLowCl",
                                "sbCautionHighCl",
                                "sbHighCl",
                                "sbLowPh",
                                "sbCautionLowPh",
                                "sbCautionHighPh",
                                "sbHighPh"
                            ]
                            readonly property list<string> fieldsPeak : [
                                "peaknum",
                                "peakstart1",
                                "peakend1",
                                "peakstart2",
                                "peakend2",
                                "midpeaknum",
                                "midpeakstart1",
                                "midpeakend1",
                                "midpeakstart2",
                                "midpeakend2",
                                "offpeakstart",
                                "offpeakend",
                                "offset",
                                "peakheater",
                                "peakfilter",
                                "peakozone",
                                "midpeakheater",
                                "midpeakfilter",
                                "midpeakozone",
                                "sat",
                                "sun",
                                "mon",
                                "tue",
                                "wed",
                                "thu",
                                "fri"
                            ]
                            readonly property list<string> fieldsPeripheral : [
                                "guid",
                                "hardwareVersion",
                                "firmwareVersion",
                                "productCode",
                                "connected"
                            ]
                            readonly property list<string> fieldsSettings : [
                                "maxFiltrationFrequency",
                                "minFiltrationFrequency",
                                "filtrationFrequency",
                                "maxFiltrationDuration",
                                "minFiltrationDuration",
                                "filtrationDuration",
                                "maxOnzenHours",
                                "minOnzenHours",
                                "onzenHours",
                                "maxOnzenCycles",
                                "minOnzenCycles",
                                "onzenCycles",
                                "maxOzoneHours",
                                "minOzoneHours",
                                "ozoneHours",
                                "maxOzoneCycles",
                                "minOzoneCycles",
                                "ozoneCycles",
                                "filterSuspension",
                                "flashLightsOnError",
                                "temperatureOffset",
                                "saunaDuration",
                                "minTemperature",
                                "maxTemperature",
                                "filtrationOffset",
                                "spaboyHours"
                            ]

                            property string currentMessageName: ""

                            function populateData(messageName) {
                                messageTableView.currentMessageName = messageName

                                const fields = messageTableView[`fields${messageName}`]
                                const proto = core[`message${messageName}`]

                                messageTableView.model.clear();
                                messageTableView.clearColumnWidths();

                                fields.forEach((field) => {
                                    messageTableView.model.appendRow({
                                        "field": field,
                                        "value": proto[field].toString()
                                    })
                                })

                                messageTableView.positionViewAtCell(Qt.point(0, 0), TableView.AlignLeft | TableView.AlignTop)
                            }

                            Component.onCompleted: {
                                messageTableView.populateData("Live")
                            }
                        }
                    }

                    Component {
                        id: viewOptionButton
                        Button {
                            property string messageName: ""
                            text: messageName
                            checked: messageTableView.currentMessageName === messageName
                            onReleased: {
                                messageTableView.populateData(messageName)
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.horizontalStretchFactor: 0
                        Layout.fillHeight: true

                        implicitWidth: 100

                        color: "#FFFFFF"

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 2

                            ButtonGroup {
                                id: viewOptionButtonGroup
                                onClicked: button => {
                                    // console.log("clicked:", button.text)
                                }
                            }
                            Repeater {
                                model: [
                                    "Clock",
                                    "Configuration",
                                    "Error",
                                    "Filter",
                                    "Information",
                                    "Live",
                                    "OnzenLive",
                                    "OnzenSettings",
                                    "Peak",
                                    "Peripheral",
                                    "Settings"
                                ]
                                Loader {
                                    Layout.fillWidth: true
                                    sourceComponent: viewOptionButton
                                    onLoaded: {
                                        item.messageName = modelData
                                        item.ButtonGroup.group = viewOptionButtonGroup
                                    }
                                }
                            }

                            Rectangle {
                                Layout.fillHeight: true
                            }
                        }
                    }
                }
            }
        }
    }
}

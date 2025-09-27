import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import Qt.labs.qmlmodels

Item {
    id: root

    Rectangle {
        anchors.fill: parent
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
                Layout.fillHeight: true
                Layout.horizontalStretchFactor: 0
                Layout.verticalStretchFactor: 1

                Layout.preferredWidth: viewOptionButtonsScrollView.contentWidth

                ScrollView {
                    id: viewOptionButtonsScrollView

                    anchors.fill: parent

                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                    ScrollBar.vertical.policy: ScrollBar.AlwaysOff
                    clip: true

                    Flickable {
                        contentWidth: viewOptionButtonsLayout.implicitWidth
                        contentHeight: viewOptionButtonsLayout.implicitHeight

                        boundsBehavior: Flickable.StopAtBounds

                        ColumnLayout {
                            id: viewOptionButtonsLayout
                            spacing: 0

                            ButtonGroup {
                                id: viewOptionButtonsGroup
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
                                delegate: Loader {
                                    Layout.fillWidth: true
                                    sourceComponent: viewOptionButton
                                    onLoaded: {
                                        item.messageName = modelData
                                        item.ButtonGroup.group = viewOptionButtonsGroup
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                // edge-border
                Layout.fillHeight: true
                width: 1
                color: "#AAAAAA"
            }

            Rectangle {
                // Layout.topMargin: 1
                // Layout.bottomMargin: 1
                // Layout.leftMargin: 1
                // Layout.rightMargin: 1

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.horizontalStretchFactor: 1
                Layout.verticalStretchFactor: 1

                ColumnLayout {
                    anchors.fill: parent

                    Rectangle {
                        Layout.topMargin: 1
                        Layout.bottomMargin: 1
                        Layout.leftMargin: 1
                        Layout.rightMargin: 1

                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.horizontalStretchFactor: 1
                        Layout.verticalStretchFactor: 1

                        HorizontalHeaderView {
                            id: messageTableHeader

                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.right: parent.right

                            clip: true

                            model: [
                                "Field",
                                "Value"
                            ]

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

                            function onMessageChanged(messageName) {
                                if (messageTableView.currentMessageName === messageName) {
                                    messageTableView.populateData(messageName)
                                }
                            }

                            Component.onCompleted: {
                                messageTableView.populateData("Live")
                            }

                            Connections {
                                target: core
                                function onMessageClockChanged() {
                                    messageTableView.onMessageChanged("Clock")
                                }
                                function onMessageConfigurationChanged() {
                                    messageTableView.onMessageChanged("Configuration")
                                }
                                function onMessageErrorChanged() {
                                    messageTableView.onMessageChanged("Error")
                                }
                                function onMessageFilterChanged() {
                                    messageTableView.onMessageChanged("Filter")
                                }
                                function onMessageInformationChanged() {
                                    messageTableView.onMessageChanged("Information")
                                }
                                function onMessageLiveChanged() {
                                    messageTableView.onMessageChanged("Live")
                                }
                                function onMessageOnzenLiveChanged() {
                                    messageTableView.onMessageChanged("OnzenLive")
                                }
                                function onMessageOnzenSettingsChanged() {
                                    messageTableView.onMessageChanged("OnzenSettings")
                                }
                                function onMessagePeakChanged() {
                                    messageTableView.onMessageChanged("Peak")
                                }
                                function onMessagePeripheralChanged() {
                                    messageTableView.onMessageChanged("Peripheral")
                                }
                                function onMessageSettingsChanged() {
                                    messageTableView.onMessageChanged("Settings")
                                }
                            }

                        }
                    }

                    Rectangle {
                        // edge-border
                        Layout.fillWidth: true
                        height: 1
                        color: "#AAAAAA"
                    }

                    Rectangle {
                        Layout.topMargin: 5
                        Layout.bottomMargin: 5
                        Layout.leftMargin: 5
                        Layout.rightMargin: 5

                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.horizontalStretchFactor: 1
                        Layout.verticalStretchFactor: 0

                        Layout.preferredHeight: refreshButton.height

                        RowLayout {
                            anchors.fill: parent
                            spacing: 0

                            Label {
                                text: {
                                    const currentMessageName = messageTableView.currentMessageName
                                    if (!currentMessageName) {
                                        return ""
                                    }

                                    const messageReceivedAt = core[`message${currentMessageName}ReceivedAt`]

                                    let messageReceivedAtFormatted = "N/A"
                                    if (!isNaN(messageReceivedAt.getTime())) {
                                        messageReceivedAtFormatted = Qt.formatDateTime(messageReceivedAt, "yyyy-MM-dd hh:mm:ss")
                                    }

                                    return `Last received at: ${messageReceivedAtFormatted}`
                                }
                            }

                            Rectangle {
                                Layout.fillWidth: true
                            }

                            Button {
                                id: refreshButton
                                implicitWidth: 120
                                text: "Refresh"
                                onReleased: {
                                    const currentMessageName = messageTableView.currentMessageName
                                    if (!currentMessageName) {
                                        return ""
                                    }
                                    console.log(`refreshing message "${currentMessageName}"`)
                                    core[`refreshMessage${currentMessageName}`]()
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: viewOptionButton

        Button {
            property string messageName: ""

            text: messageName
            checked: messageTableView.currentMessageName === messageName
            onReleased: messageTableView.populateData(messageName)
        }
    }
}

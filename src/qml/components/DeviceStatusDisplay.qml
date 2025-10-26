import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import asdc.types.sockets

Item {
    id: root

    function formatMillisecondsToHHMMSS (ms) {
        if (!ms || ms < 0) {
            return null
        }

        let totalSeconds = Math.floor(ms / 1000)

        const hours = Math.floor(totalSeconds / 3600)
        totalSeconds %= 3600

        const minutes = Math.floor(totalSeconds / 60)
        totalSeconds %= 60

        const seconds = totalSeconds

        const pad = (num) => String(num).padStart(2, '0')

        return `${pad(hours)}:${pad(minutes)}:${pad(seconds)}`
    }

    GridLayout {
        anchors.fill: parent
        columns: 4
        rowSpacing: 5
        columnSpacing: 5

        Item {
            Layout.row: 0
            Layout.column: 0
            Layout.columnSpan: 4
            Layout.fillHeight: true
        }

        Label {
            Layout.row: 1
            Layout.column: 0
            text: "Last Check:"
        }
        Label {
            Layout.row: 1
            Layout.column: 1
            text: core.messageLiveReceivedAt || "N/A"
        }

        Label {
            Layout.row: 2
            Layout.column: 0
            text: "Next Check:"
        }
        Label {
            Layout.row: 2
            Layout.column: 1
            // text: formatMillisecondsToHHMMSS(deviceClient.pollingProcessRemainingTime) || "N/A"
            text: "N/A"
        }

        Item {
            Layout.row: 1
            Layout.column: 2
            Layout.rowSpan: 2
            Layout.fillWidth: true
        }

        Button {
            id: refreshButton
            Layout.row: 1
            Layout.column: 3
            Layout.rowSpan: 2
            Layout.alignment: Qt.AlignVCenter
            text: "Refresh"
            // icon.source: "qrc:/assets/icons/refresh.svg"
            enabled: core.networkState === SocketState.ConnectedState
            onReleased: {
                core.refreshMessageLive()
                core.refreshMessageOnzenLive()
                core.refreshMessageError()
            }
        }
    }
}

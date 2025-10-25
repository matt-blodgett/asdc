import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

import asdc.types.sockets

Item {
    id: root

    signal startupFinished(bool success)

    property list<string> foundHosts : []

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.horizontalStretchFactor: 1
            Layout.verticalStretchFactor: 1
            Rectangle {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                implicitWidth: 20
                color: "#3879B5"
            }
        }
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 160

            ColumnLayout {
                anchors.fill: parent
                spacing: 6

                Label {
                    Layout.fillWidth: true
                    verticalAlignment: Qt.AlignVCenter
                    horizontalAlignment: Qt.AlignHCenter
                    text: "asdc"
                    font.pixelSize: 36
                }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.verticalStretchFactor: 1
                }
                TextField {
                    id: hostIpAddressTextField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    verticalAlignment: Qt.AlignVCenter
                    leftPadding: 5
                    placeholderText: "Host IP Address"
                    text: ""
                    enabled: !core.discoveryWorking
                    font.pixelSize: 18
                }
                Button {
                    id: autoConnectButton
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    text: "Connect"
                    enabled: !core.discoveryWorking && hostIpAddressTextField.text !== ""
                    onReleased: {
                        console.log(foundHosts)
                        // startupFinished(true)
                    }
                }
                Button {
                    id: scanConnectButton
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    text: "Scan Network"
                    enabled: !core.discoveryWorking
                    onReleased: {
                        foundHosts = []
                        core.discoverySearch()
                    }
                }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.verticalStretchFactor: 2
                }
            }
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.horizontalStretchFactor: 1
            Layout.verticalStretchFactor: 1
            Rectangle {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                implicitWidth: 20
                color: "#3879B5"
            }
        }
    }

    Connections {
        target: core
        function onDiscoveryHostFound(host) {
            foundHosts.push(host)
        }
        function onDiscoveryFinishedSearch() {
            console.log(foundHosts)
            if (foundHosts.length === 0) {
                console.log("No hosts found!")
            } else {
                if (foundHosts.length === 1) {
                    console.log("Found one host, using to connect")
                } else if (foundHosts.length > 1) {
                    console.log("Found multiple hosts, using the first")
                }

                hostIpAddressTextField.text = foundHosts[0]
            }
        }
    }

    Settings {
        category: "network"
        property alias hostIpAddress: hostIpAddressTextField.text
    }
}

import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import asdc.types.sockets

Item {
    id: root

    Component {
        id: loadingIcon
        Image {
            width: 24; height: 24
            source: "qrc:/assets/icons/loader-dots.svg"
            RotationAnimator on rotation { running: true; loops: Animation.Infinite; duration: 2000; from: 0; to: 360 }
        }
    }
    Component {
        id: successIcon
        Image {
            width: 24; height: 24
            source: "qrc:/assets/icons/check-circle.svg"
        }
    }
    Component {
        id: errorIcon
        Image {
            width: 24; height: 24
            source: "qrc:/assets/icons/alert-circle.svg"
        }
    }

    ColumnLayout {
        anchors.fill: parent
        // anchors.topMargin: 5
        // anchors.bottomMargin: 5
        // anchors.leftMargin: 10
        // anchors.rightMargin: 10

        Rectangle {
            id: connectionContainer

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.horizontalStretchFactor: 0
            Layout.verticalStretchFactor: 1

            implicitWidth: hostIpAddressInput.implicitWidth + connectButton.implicitWidth + connectionStatusLabel.implicitWidth + 100;

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
                            if (core.networkState === SocketState.UnconnectedState) {
                                return "Connect"
                            } else if (core.networkState === SocketState.ConnectedState) {
                                return "Disconnect"
                            }
                            return "Loading..."
                        }
                        enabled: {
                            if (core.networkState === SocketState.UnconnectedState && hostIpAddressInput.text) {
                                return true
                            }
                            return core.networkState === SocketState.ConnectedState
                        }
                        onReleased: {
                            if (core.networkState === SocketState.UnconnectedState) {
                                const host = hostIpAddressInput.text
                                core.networkConnectToDevice(host)
                            } else if (core.networkState === SocketState.ConnectedState) {
                                core.networkDisconnectFromDevice()
                            }
                        }
                    }
                    Label {
                        id: connectionStatusLabel
                        text: {
                            if (core.networkState === SocketState.UnconnectedState) {
                                return "Disconnected"
                            } else if (core.networkState === SocketState.HostLookupState || core.networkState === SocketState.ConnectingState) {
                                return "Connecting..."
                            } else if (core.networkState === SocketState.ConnectedState) {
                                return "Connected"
                            } else if (core.networkState === SocketState.ClosingState) {
                                return "Disconnecting..."
                            }
                            return "Unknown"
                        }
                    }
                    Loader {
                        id: iconLoader
                        sourceComponent: {
                            if (core.networkState === SocketState.UnconnectedState) {
                                return errorIcon
                            } else if (core.networkState === SocketState.HostLookupState || core.networkState === SocketState.ConnectingState) {
                                return loadingIcon
                            } else if (core.networkState === SocketState.ConnectedState) {
                                return successIcon
                            } else if (core.networkState === SocketState.ClosingState) {
                                return loadingIcon
                            }
                            return errorIcon
                        }
                    }
                }
            }
        }

        Rectangle {

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.horizontalStretchFactor: 0
            Layout.verticalStretchFactor: 1

            RowLayout {
                Button {
                    text: "test discovery"
                    enabled: !core.discoveryWorking
                    onReleased: {
                        core.testDiscovery()
                    }
                }
                Label {
                    text: core.discoveryWorking ? "WORKING" : "not working"
                }
            }
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 1
        }
    }

    Settings {
        category: "client"
        property alias hostIpAddress: hostIpAddressInput.text
    }
}

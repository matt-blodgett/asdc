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
            width: 26; height: 26
            source: "/qt/qml/asdc/qml/assets/icons/progress_circle.svg"
            RotationAnimator on rotation { running: true; loops: Animation.Infinite; duration: 1000; from: 0; to: 360 }
        }
    }
    Component {
        id: successIcon

        Image {
            width: 26; height: 26
            source: "/qt/qml/asdc/qml/assets/icons/check_circle.svg"
        }
    }
    Component {
        id: errorIcon

        Image {
            width: 26; height: 26
            source: "/qt/qml/asdc/qml/assets/icons/error_circle.svg"
        }
    }

    Rectangle {
        anchors.fill: parent
        // anchors.topMargin: 5
        // anchors.bottomMargin: 5
        // anchors.leftMargin: 10
        // anchors.rightMargin: 10

        Rectangle {
            id: connectionContainer

            // Layout.fillWidth: true
            // Layout.horizontalStretchFactor: 0
            // Layout.fillHeight: true

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
                    Loader {
                        id: iconLoader
                        sourceComponent: {
                            if (core.clientState === SocketState.UnconnectedState) {
                                return errorIcon
                            } else if (core.clientState === SocketState.HostLookupState || core.clientState === SocketState.ConnectingState) {
                                return loadingIcon
                            } else if (core.clientState === SocketState.ConnectedState) {
                                return successIcon
                            } else if (core.clientState === SocketState.ClosingState) {
                                return loadingIcon
                            }
                            return errorIcon
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
    }

    Settings {
        category: "client"
        property alias hostIpAddress: hostIpAddressInput.text
    }
}

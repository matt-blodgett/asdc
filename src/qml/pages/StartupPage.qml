import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

import asdc.types.sockets

import "../components"

Item {
    id: root

    property list<string> foundHosts : []
    readonly property list<string> startupMessageTypes : [
        "Configuration",
        "Settings",
        "Information",
        "Peripheral",
        "Error",
        "OnzenSettings",
        "OnzenLive",
        "Live"
    ]

    signal startupFinished()

    function startNetworkConnectToDevice() {
        const host = hostIpAddressTextField.text

        loadingDialog.processStatus = "loading"
        loadingDialog.displayMessage = `Connecting to host "${host}"...`
        loadingDialog.visible = true

        core.networkConnectToDevice(host)
    }
    function startDiscoverySearch() {
        loadingDialog.processStatus = "loading"
        loadingDialog.displayMessage = "Scanning network..."
        loadingDialog.visible = true

        foundHosts = []
        core.discoverySearch()
    }
    function finishIfAllStartupMessagesReceived() {
        for (let messageType of startupMessageTypes) {
            if (isNaN(core[`message${messageType}ReceivedAt`].getTime())) {
                console.log(`missing ${messageType}`)
                return
            }
        }
        loadingDialog.visible = false
        startupFinished()
    }

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
                    id: manualConnectButton
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    text: "Connect"
                    enabled: !core.discoveryWorking && hostIpAddressTextField.text !== "" && core.networkState === SocketState.UnconnectedState
                    onReleased: startNetworkConnectToDevice()
                }
                Button {
                    id: scanConnectButton
                    Layout.fillWidth: true
                    Layout.preferredHeight: 36
                    text: "Scan Network"
                    enabled: !core.discoveryWorking
                    onReleased: startDiscoverySearch()
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

    LoadingDialog {
        id: loadingDialog
        anchors.centerIn: parent

        width: 275
        height: 100

        visible: false

        onCancelPressed: {
            loadingDialog.visible = false
        }
        onContinuePressed: {
            loadingDialog.visible = false
        }
    }

    Connections {
        target: core
        enabled: root.visible

        function onDiscoveryHostFound(host) {
            foundHosts.push(host)
        }
        function onDiscoveryFinishedSearch() {
            console.log(foundHosts)
            if (foundHosts.length === 0) {
                console.log("No hosts found!")
                loadingDialog.processStatus = "error"
                loadingDialog.displayMessage = "Could not find a connected spa!"
            } else {
                if (foundHosts.length > 1) {
                    console.log("Found multiple hosts - using the first")
                }
                hostIpAddressTextField.text = foundHosts[0]
                startNetworkConnectToDevice()
            }
        }

        function onNetworkConnected() {
            loadingDialog.processStatus = "loading"
            loadingDialog.displayMessage = "Retrieving spa configuration..."

            for (let messageType of startupMessageTypes) {
                core[`refreshMessage${messageType}`]()
            }
        }
        function onNetworkErrorOccurred() {
            console.log(core.networkError)
            loadingDialog.processStatus = "error"
            loadingDialog.displayMessage = "Connection failed!"
        }

        function onMessageConfigurationChanged() {
            finishIfAllStartupMessagesReceived()
        }
        function onMessageSettingsChanged() {
            finishIfAllStartupMessagesReceived()
        }
        function onMessageInformationChanged() {
            finishIfAllStartupMessagesReceived()
        }
        function onMessageOnzenSettingsChanged() {
            finishIfAllStartupMessagesReceived()
        }
        function onMessageOnzenLiveChanged() {
            finishIfAllStartupMessagesReceived()
        }
        function onMessageLiveChanged() {
            finishIfAllStartupMessagesReceived()
        }
    }

    Settings {
        category: "network"
        property alias hostIpAddress: hostIpAddressTextField.text
    }
}

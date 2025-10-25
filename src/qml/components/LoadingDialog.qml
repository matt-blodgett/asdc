import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// Window {
Rectangle {
    id: root

    property string processStatus : ""
    property string displayMessage : ""

    signal cancelPressed()
    signal continuePressed()

    border.color: "#888888"
    border.width: 1
    color: "#DDDDDD"


    // title: "Loading..."
    // modal: true
    // focus: true
    // closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    // popupType: Popup.Window

    // modality: Qt.WindowModal
    // flags: Qt.FramelessWindowHint
    // flags: Qt.CustomizeWindowHint

    // flags: Qt.WindowSystemMenuHint | Qt.WindowCloseButtonHint
    // flags: Qt.CustomizeWindowHint | Qt.WindowSystemMenuHint | Qt.WindowCloseButtonHint

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

    Column {
        anchors.centerIn: parent
        anchors.bottom: parent.bottom
        spacing: 16
        width: parent.width - 40

        Row {
            spacing: 8
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter

            Label {
                id: displayMessageLabel
                text: displayMessage
                width: parent.width - 26
            }
            Loader {
                id: iconLoader
                sourceComponent: {
                    if (processStatus === "loading") {
                        return loadingIcon
                    } else if (processStatus === "success") {
                        return successIcon
                    }
                    return errorIcon
                }
            }
        }

        Row {
            spacing: 16
            width: parent.width

            Button {
                text: "Cancel"
                width: (parent.width - 16) / 2
                onReleased: {
                    console.log("cancelled")
                    root.cancelPressed()
                    root.visible = false
                }
            }
            Button {
                text: "Continue"
                width: (parent.width - 16) / 2
                enabled: processStatus !== "loading"
                onReleased: {
                    console.log("continue")
                    root.continuePressed()
                }
            }
        }
    }
}

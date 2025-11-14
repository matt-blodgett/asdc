import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root

    title: "About"

    minimumWidth: 400
    minimumHeight: 300
    maximumWidth: 400
    maximumHeight: 300

    modality: Qt.ApplicationModal
    flags: Qt.Window |
           Qt.CustomizeWindowHint |
           Qt.WindowTitleHint |
           Qt.WindowCloseButtonHint |
           Qt.WindowSystemMenuHint

    readonly property string gitRepositoryUrl : "https://github.com/matt-blodgett/asdc"

    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: 10
        anchors.bottomMargin: 10
        anchors.leftMargin: 10
        anchors.rightMargin: 10

        spacing: 0

        ColumnLayout {
            spacing: 4

            Label {
                text: "asdc"
                font.pixelSize: 22
                font.bold: true
            }
            Label {
                text: 'Version: ' + appBuildInfo.version
            }
            Label {
                text: 'Build Date: ' + appBuildInfo.date
            }
            Label {
                text: 'Build Type: ' + appBuildInfo.type
            }
            Label {
                text: 'OS: ' + appBuildInfo.os
            }
            Label {
                text: 'Arch: ' + appBuildInfo.arch
            }
            Label {
                text: 'Compiler: ' + appBuildInfo.compiler
            }
            Label {
                text: 'Qt Version: ' + appBuildInfo.qtVersion
            }
            RowLayout {
                spacing: 0

                Label {
                    text: "Github: "
                }
                Text {
                    text: `<html><a href="${gitRepositoryUrl}">${gitRepositoryUrl}</a></html>`
                    onLinkActivated: Qt.openUrlExternally(link)
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.NoButton
                        cursorShape: Qt.PointingHandCursor
                    }
                }
            }
        }
        Item {
            Layout.fillHeight: true
        }
        RowLayout {
            spacing: 0

            Item {
                Layout.fillWidth: true
            }
            Button {
                text: "Close"
                onReleased: root.close()
            }
        }
    }
}

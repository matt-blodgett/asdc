import QtCore
import QtQuick
import QtQuick.Controls.Fusion
import QtQuick.Controls
import QtQuick.Layouts

import "pages"
import "components"

ApplicationWindow {
    id: mainWindow

    title: "asdc"

    visible: false
    minimumWidth: 330
    minimumHeight: 300

    menuBar: MenuBar {
        // background: Rectangle {
        //     color: "#e0e0e0"
        // }
        Menu {
            title: qsTr("File")
            MenuItem {
                text: "Test Mode"
                enabled: !startupPageFadeaway.running && startupPage.visible
                onTriggered: {
                    core.testMode()
                    startupPageFadeaway.running = true
                }
            }
            MenuSeparator {}
            MenuItem {
                text: "Exit"
                onTriggered: Qt.quit()
            }
        }
        Menu {
            title: qsTr("View")
            Menu {
                title: qsTr("Theme")
                MenuItem {
                    text: "Light"
                    checkable: true
                    checked: AppStyle.colorScheme === Qt.Light
                    onTriggered: {
                        AppStyle.setColorScheme(Qt.Light)
                    }
                }
                MenuItem {
                    text: "Dark"
                    checkable: true
                    checked: AppStyle.colorScheme === Qt.Dark
                    onTriggered: {
                        AppStyle.setColorScheme(Qt.Dark)
                    }
                }
                MenuItem {
                    text: "System"
                    checkable: true
                    checked: AppStyle.colorScheme === Qt.Unknown
                    onTriggered: {
                        AppStyle.setColorScheme(Qt.Unknown)
                    }
                }
            }
        }
        Menu {
            title: qsTr("Help")
            MenuItem {
                text: "About"
                onTriggered: {
                    aboutDialog.show()
                }
            }
        }
    }

    AboutDialog {
        id: aboutDialog
    }


    // MouseArea {
    //     anchors.fill: parent
    //     onClicked: {
    //         console.log("base mouse area pressed")
    //         focus = true
    //     }
    // }


    MainPage {
        id: mainPage
        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 6
        anchors.topMargin: 4
        anchors.bottomMargin: 8
        visible: false
    }
    StartupPage {
        id: startupPage
        anchors.fill: parent
        visible: true
        onStartupFinished: {
            startupPageFadeaway.running = true
        }
    }
    PropertyAnimation {
        id: startupPageFadeaway
        target: startupPage
        property: "opacity"
        to: 0
        duration: 500
        onFinished: {
            startupPage.visible = false
            mainPage.visible = true
        }
    }

    footer: ToolBar {
        visible: startupPageFadeaway.running || !startupPage.visible

        RowLayout {
            anchors.fill: parent
            spacing: 0

            ToolButton {
                Layout.fillWidth: true
                Layout.horizontalStretchFactor: 1
                text: "Home"
                checked: mainPage.currentPageIndex === 0
                onReleased: mainPage.currentPageIndex = 0
            }
            ToolButton {
                Layout.fillWidth: true
                Layout.horizontalStretchFactor: 1
                text: "Tools"
                checked: mainPage.currentPageIndex === 1
                onReleased: mainPage.currentPageIndex = 1
            }
            ToolButton {
                Layout.fillWidth: true
                Layout.horizontalStretchFactor: 1
                text: "Settings"
                checked: mainPage.currentPageIndex === 2
                onReleased: mainPage.currentPageIndex = 2
            }
        }
    }

    Settings {
        category: "MainWindow"
        property alias x: mainWindow.x
        property alias y: mainWindow.y
        property alias width: mainWindow.width
        property alias height: mainWindow.height
    }

    // Connections {
    //     target: AppStyle
    //     function onColorSchemeChanged() {
    //         console.log(AppStyle.colorScheme)
    //     }
    //     function onDarkModeChanged() {
    //         console.log(AppStyle.darkMode)
    //     }
    // }

    Component.onCompleted: {
        AppStyle.setColorScheme(AppStyle.colorScheme)
        mainWindow.visible = true
    }
}

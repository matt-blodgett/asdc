import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "pages"

ApplicationWindow {
    id: mainWindow

    title: "asdc"
    visible: true
    minimumWidth: 330
    minimumHeight: 300

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: "Test Mode"
                onTriggered: {
                    // core.testMode()
                    startupPageFadeaway.running = true
                }
            }
            MenuSeparator {}
            MenuItem {
                text: "Exit"
                onTriggered: Qt.quit()
            }
        }
    }


    MouseArea {
        anchors.fill: parent
        onClicked: {
            console.log("base mouse area pressed")
            focus = true
        }
    }


    MainPage {
        id: mainPage
        anchors.fill: parent
        visible: true
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
                onReleased: mainPage.currentPageIndex = 0
                checked: mainPage.currentPageIndex === 0
            }
            ToolButton {
                Layout.fillWidth: true
                Layout.horizontalStretchFactor: 1
                text: "Tools"
                onReleased: mainPage.currentPageIndex = 1
                checked: mainPage.currentPageIndex === 1
            }
            ToolButton {
                Layout.fillWidth: true
                Layout.horizontalStretchFactor: 1
                text: "Settings"
                onReleased: mainPage.currentPageIndex = 2
                checked: mainPage.currentPageIndex === 2
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
}

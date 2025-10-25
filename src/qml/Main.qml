import QtCore
import QtQuick
import QtQuick.Controls

import "pages"

ApplicationWindow {
    id: mainWindow

    title: "asdc"
    visible: true
    minimumWidth: 320
    minimumHeight: 300

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: "Test Discovery"
                onTriggered: core.testDiscovery()
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

    StartupPage {
        id: startupPage
        anchors.fill: parent
        visible: true
        onStartupFinished: function(success) {
            startupPage.visible = !success
            mainPage.visible = success
        }
    }
    MainPage {
        id: mainPage
        anchors.fill: parent
        visible: false
    }

    Settings {
        category: "mainWindow"
        property alias x: mainWindow.x
        property alias y: mainWindow.y
        property alias width: mainWindow.width
        property alias height: mainWindow.height
    }
}

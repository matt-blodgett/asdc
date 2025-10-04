import QtCore
import QtQuick
import QtQuick.Controls

import "pages"

ApplicationWindow {
    id: mainWindow

    title: "asdc"
    visible: true
    minimumWidth: 300
    minimumHeight: 250

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

    MainPage {
        anchors.fill: parent
    }

    Settings {
        category: "mainWindow"
        property alias x: mainWindow.x
        property alias y: mainWindow.y
        property alias width: mainWindow.width
        property alias height: mainWindow.height
    }
}

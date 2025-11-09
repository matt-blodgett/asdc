import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

import asdc.types.sockets

// import src.qml.components
import "../components"

Item {
    id: root

    property int currentPageIndex : -1

    ColumnLayout {
        anchors.fill: parent

        visible: currentPageIndex < 0

        Button {
            text: "Message Viewer"
            onReleased: currentPageIndex = 0
        }
        Button {
            text: "Database Viewer"
            onReleased: currentPageIndex = 1
        }
        Rectangle {
            Layout.fillHeight: true
        }
    }

    ColumnLayout {
        anchors.fill: parent


        visible: currentPageIndex >= 0

        Button {
            text: "Back"
            onReleased: currentPageIndex = -1
        }

        StackLayout {
            currentIndex: currentPageIndex

            MessageViewerPage {}
            SqlBrowserPage {}
        }
    }
}

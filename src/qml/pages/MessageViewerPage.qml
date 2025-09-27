import QtQuick

import "../components"

Item {
    id: root

    Rectangle {
        anchors.fill: parent

        MessageViewer {
            anchors.fill: parent
            // anchors.topMargin: 1
            // anchors.bottomMargin: 1
            // anchors.leftMargin: 1
            // anchors.rightMargin: 1
        }
    }
}

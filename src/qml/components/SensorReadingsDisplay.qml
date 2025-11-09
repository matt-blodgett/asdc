import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import src.qml.proto

Item {
    id: root

    function sectionColor(currentStatus, sectionStatus) {
        if (currentStatus !== sectionStatus) {
            return "transparent"
        }
        if (currentStatus === OnzenLive.COLOR_LOW || currentStatus === OnzenLive.COLOR_HIGH) {
            return "#F54518"
        }
        if (currentStatus === OnzenLive.COLOR_CAUTION_LOW || currentStatus === OnzenLive.COLOR_CAUTION_HIGH) {
            return "#F28735"
        }
        if (currentStatus === OnzenLive.COLOR_OK) {
            return "#48D95F"
        }
        return "transparent"
    }

    // Component {
    //     id: header
    //     Rectangle {
    //         property string label: ""
    //         Layout.fillWidth: true
    //         Layout.horizontalStretchFactor: 1
    //         Layout.alignment: Qt.AlignHCenter
    //         Label {
    //             anchors.fill: parent
    //             text: label
    //             horizontalAlignment: Text.AlignHCenter
    //         }
    //     }
    // }

    GridLayout {
        anchors.fill: parent

        columns: 6
        rowSpacing: 4
        columnSpacing: 2

        // Headers
        Label { text: "" }
        Rectangle {
            Layout.bottomMargin: 5
            Layout.fillWidth: true
            Layout.horizontalStretchFactor: 1
            Layout.alignment: Qt.AlignHCenter
            Label {
                anchors.fill: parent
                text: "LOW"
                horizontalAlignment: Text.AlignHCenter
            }
        }
        // Loader {
        //     id: myLoader
        //     sourceComponent: header
        //     onLoaded: {
        //         item.label = "LOW"
        //     }
        // }
        Rectangle {
            Layout.fillWidth: true
            Layout.horizontalStretchFactor: 1
            Layout.alignment: Qt.AlignHCenter
            Label {
                anchors.fill: parent
                text: ""
                horizontalAlignment: Text.AlignHCenter
            }
        }
        Rectangle {
            Layout.bottomMargin: 5
            Layout.fillWidth: true
            Layout.horizontalStretchFactor: 1
            Layout.alignment: Qt.AlignHCenter
            Label {
                anchors.fill: parent
                text: "OK"
                horizontalAlignment: Text.AlignHCenter
            }
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.horizontalStretchFactor: 1
            Layout.alignment: Qt.AlignHCenter
            Label {
                anchors.fill: parent
                text: ""
                horizontalAlignment: Text.AlignHCenter
            }
        }
        Rectangle {
            Layout.bottomMargin: 5
            Layout.fillWidth: true
            Layout.horizontalStretchFactor: 1
            Layout.alignment: Qt.AlignHCenter
            Label {
                anchors.fill: parent
                text: "HIGH"
                horizontalAlignment: Text.AlignHCenter
            }
        }

        // ORP Row
        Label {
            Layout.rightMargin: 5
            text: `CL ${core.messageOnzenLive.orp}`
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.horizontalStretchFactor: 1
            // width: 24
            height: 24
            color: sectionColor(core.messageOnzenLive.orpColor, OnzenLive.COLOR_LOW)
            border.color: "#FFFFFF"; border.width: 1; radius: 2
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.horizontalStretchFactor: 1
            // width: 24
            height: 24
            color: sectionColor(core.messageOnzenLive.orpColor, OnzenLive.COLOR_CAUTION_LOW)
            border.color: "#FFFFFF"; border.width: 1; radius: 2
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.horizontalStretchFactor: 1
            // width: 24
            height: 24
            color: sectionColor(core.messageOnzenLive.orpColor, OnzenLive.COLOR_OK)
            border.color: "#FFFFFF"; border.width: 1; radius: 2
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.horizontalStretchFactor: 1
            // width: 24
            height: 24
            color: sectionColor(core.messageOnzenLive.orpColor, OnzenLive.COLOR_CAUTION_HIGH)
            border.color: "#FFFFFF"; border.width: 1; radius: 2
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.horizontalStretchFactor: 1
            // width: 24
            height: 24
            color: sectionColor(core.messageOnzenLive.orpColor, OnzenLive.COLOR_HIGH)
            border.color: "#FFFFFF"; border.width: 1; radius: 2
        }

        // pH Row
        Label {
            Layout.rightMargin: 5
            text: `pH ${core.messageOnzenLive.ph100}`
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.horizontalStretchFactor: 1
            // width: 24
            height: 24
            color: sectionColor(core.messageOnzenLive.phColor, OnzenLive.COLOR_LOW)
            border.color: "#FFFFFF"; border.width: 1; radius: 2
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.horizontalStretchFactor: 1
            // width: 24
            height: 24
            color: sectionColor(core.messageOnzenLive.phColor, OnzenLive.COLOR_CAUTION_LOW)
            border.color: "#FFFFFF"; border.width: 1; radius: 2
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.horizontalStretchFactor: 1
            // width: 24
            height: 24
            color: sectionColor(core.messageOnzenLive.phColor, OnzenLive.COLOR_OK)
            border.color: "#FFFFFF"; border.width: 1; radius: 2
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.horizontalStretchFactor: 1
            // width: 24
            height: 24
            color: sectionColor(core.messageOnzenLive.phColor, OnzenLive.COLOR_CAUTION_HIGH)
            border.color: "#FFFFFF"; border.width: 1; radius: 2
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.horizontalStretchFactor: 1
            // width: 24
            height: 24
            color: sectionColor(core.messageOnzenLive.phColor, OnzenLive.COLOR_HIGH)
            border.color: "#FFFFFF"; border.width: 1; radius: 2
        }
    }

    // Rectangle {
    //     anchors.fill: parent
    //     color: "transparent"
    //     border.color: "#ACACAC"
    //     border.width: 1
    //     radius: 2
    //     z: -1
    // }
}

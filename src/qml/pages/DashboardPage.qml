import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../components"

Item {
    id: root

    Rectangle {
        anchors.fill: parent
        color: "#ACACAC"
        // color: "transparent"
        // border.color: "#000000"
        // border.width: 1
        // radius: 2
        // z: -1
    }

    Item {
        anchors.fill: parent
        anchors.topMargin: 5
        anchors.bottomMargin: 5
        anchors.leftMargin: 10
        anchors.rightMargin: 10

        ColumnLayout {
            anchors.fill: parent
            spacing: 5

            TemperatureControls {
                id: temperatureControls
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.horizontalStretchFactor: 1
                Layout.verticalStretchFactor: 1
            }

            FeatureControls {
                id: featureControls
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.horizontalStretchFactor: 1
                Layout.verticalStretchFactor: 1
            }

            SensorReadingsDisplay {
                id: sensorReadingsDisplay
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.horizontalStretchFactor: 1
                Layout.verticalStretchFactor: 1
            }
        }
    }
}

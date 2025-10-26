import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "../components"

Item {
    id: root

    ColumnLayout {
        anchors.fill: parent
        spacing: 5

        TemperatureControls {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.horizontalStretchFactor: 1
            Layout.verticalStretchFactor: 1
            Layout.minimumWidth: 300
            Layout.minimumHeight: 75
            Layout.maximumWidth: 400
            Layout.maximumHeight: 125
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }
        Item {
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 1
            Layout.maximumHeight: 10
        }
        FeatureControls {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.horizontalStretchFactor: 1
            Layout.verticalStretchFactor: 1
            Layout.minimumWidth: 300
            Layout.minimumHeight: 75
            Layout.maximumWidth: 400
            Layout.maximumHeight: 125
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }
        Item {
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 1
            Layout.maximumHeight: 10
        }
        SensorReadingsDisplay {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.horizontalStretchFactor: 1
            Layout.verticalStretchFactor: 1
            Layout.minimumWidth: 300
            Layout.minimumHeight: 75
            Layout.maximumWidth: 400
            Layout.maximumHeight: 125
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        }
        Item {
            Layout.fillHeight: true
            Layout.verticalStretchFactor: 1
        }
    }

    // Rectangle {
    //     anchors.fill: parent
    //     color: "#ACACAC"
    //     // color: "transparent"
    //     // border.color: "#000000"
    //     // border.width: 1
    //     // radius: 2
    //     // z: -1
    // }
}

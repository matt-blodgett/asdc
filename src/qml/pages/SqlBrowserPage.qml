import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

import asdc.types.models

Item {
    id: root

    Rectangle {
        anchors.fill: parent

        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            // The SQL model: roles will be the column names (id, created_at, name)
            SqlTableModel {
                id: sqlModel
                table: "message_live"
            }

            // // Simple header row (strings match the roles / columns)
            // RowLayout {
            //     spacing: 1
            //     Rectangle { color: "#2f2f2f"; implicitHeight: 32; Layout.preferredWidth: 70
            //         Text { anchors.centerIn: parent; text: "id"; color: "white" } }
            //     Rectangle { color: "#2f2f2f"; implicitHeight: 32; Layout.preferredWidth: 200
            //         Text { anchors.centerIn: parent; text: "created_at"; color: "white" } }
            //     Rectangle { color: "#2f2f2f"; implicitHeight: 32; Layout.fillWidth: true
            //         Text { anchors.centerIn: parent; text: "temperature_fahrenheit"; color: "white" } }
            // }

            HorizontalHeaderView {
                id: messageTableHeader

                Layout.fillWidth: true
                // Layout.fillHeight: true

                // anchors.left: parent.left
                // anchors.top: parent.top
                // anchors.right: parent.right

                clip: true

                // model: [
                //     "id",
                //     "created_at",
                //     "temperature_fahrenheit"
                // ]

                syncView: table
            }

            // The table
            TableView {
                id: table

                Layout.fillWidth: true
                Layout.fillHeight: true

                rowSpacing: 0
                columnSpacing: 0

                resizableColumns: true
                boundsBehavior: Flickable.StopAtBounds

                clip: true

                model: sqlModel

                delegate: TableViewDelegate {
                    leftPadding: 5
                    rightPadding: 5
                    topPadding: 5
                    bottomPadding: 5
                }

                // columnWidthProvider: function(column) {
                //     return 50;
                // }

                // model: SqlTableModel {
                //     table: "message_live"
                // }

                // delegate: Rectangle {
                //     implicitHeight: 28
                //     color: (row % 2 === 0) ? "#1e1e1e" : "#252525"

                //     Text {
                //         text: modelData
                //     }
                // }


                // Tell TableView how wide each column should be
                // columnWidthProvider: function(col) {
                //     switch (col) {
                //     case 0: return 70;      // id
                //     case 1: return 200;     // created_at
                //     default: return table.width - 70 - 200 - (2 * columnSpacing);
                //     }
                // }

                // One delegate per *cell*. We switch on 'column' to choose which role to show.
                // delegate: Rectangle {
                //     implicitHeight: 28
                //     color: (row % 2 === 0) ? "#1e1e1e" : "#252525"

                //     Text {
                //         anchors.centerIn: parent
                //         text: (function() {
                //             switch (column) {
                //             case 0: return model.id;
                //             case 1: return model.created_at;
                //             case 2: return model.temperature_fahrenheit;
                //             default: return "";
                //             }
                //         })()
                //         color: "#e0e0e0"
                //         elide: Text.ElideRight
                //     }
                // }
            }
        }
    //     ColumnLayout {
    //         anchors.fill: parent
    //         spacing: 0

    //         // The SQL model: roles will be the column names (id, created_at, name)
    //         SqlQueryModel {
    //             id: sqlModel
    //             query: "SELECT id, created_at, temperature_fahrenheit FROM message_live ORDER BY id ASC"
    //         }

    //         // // Simple header row (strings match the roles / columns)
    //         // RowLayout {
    //         //     spacing: 1
    //         //     Rectangle { color: "#2f2f2f"; implicitHeight: 32; Layout.preferredWidth: 70
    //         //         Text { anchors.centerIn: parent; text: "id"; color: "white" } }
    //         //     Rectangle { color: "#2f2f2f"; implicitHeight: 32; Layout.preferredWidth: 200
    //         //         Text { anchors.centerIn: parent; text: "created_at"; color: "white" } }
    //         //     Rectangle { color: "#2f2f2f"; implicitHeight: 32; Layout.fillWidth: true
    //         //         Text { anchors.centerIn: parent; text: "temperature_fahrenheit"; color: "white" } }
    //         // }

    //         HorizontalHeaderView {
    //             id: messageTableHeader

    //             // anchors.left: parent.left
    //             // anchors.top: parent.top
    //             // anchors.right: parent.right

    //             clip: true

    //             model: [
    //                 "id",
    //                 "created_at",
    //                 "temperature_fahrenheit"
    //             ]

    //             syncView: table
    //         }

    //         // The table
    //         TableView {
    //             id: table

    //             Layout.fillWidth: true
    //             Layout.fillHeight: true

    //             rowSpacing: 0
    //             columnSpacing: 0

    //             resizableColumns: true
    //             boundsBehavior: Flickable.StopAtBounds

    //             clip: true

    //             model: sqlModel

    //             // Tell TableView how wide each column should be
    //             columnWidthProvider: function(col) {
    //                 switch (col) {
    //                 case 0: return 70;      // id
    //                 case 1: return 200;     // created_at
    //                 default: return table.width - 70 - 200 - (2 * columnSpacing);
    //                 }
    //             }

    //             // One delegate per *cell*. We switch on 'column' to choose which role to show.
    //             delegate: Rectangle {
    //                 implicitHeight: 28
    //                 color: (row % 2 === 0) ? "#1e1e1e" : "#252525"

    //                 Text {
    //                     anchors.centerIn: parent
    //                     text: (function() {
    //                         switch (column) {
    //                         case 0: return model.id;
    //                         case 1: return model.created_at;
    //                         case 2: return model.temperature_fahrenheit;
    //                         default: return "";
    //                         }
    //                     })()
    //                     color: "#e0e0e0"
    //                     elide: Text.ElideRight
    //                 }
    //             }
    //         }
    //     }

    }
}

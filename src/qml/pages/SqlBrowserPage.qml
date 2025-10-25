import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.qmlmodels

import asdc.types.models

Item {
    id: root

    function applyFilters() {
        sqlModel.applyFilters()
        sqlTableView.positionViewAtCell(Qt.point(0, 0), TableView.AlignLeft | TableView.AlignTop)
    }

    SqlTableModel {
        id: sqlModel
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.topMargin: 5
            Layout.bottomMargin: 5
            Layout.leftMargin: 5
            Layout.rightMargin: 5

            spacing: 4

            Label {
                text: "Table"
            }
            ComboBox {
                id: tableComboBox
                model: [
                    { text: "ProcessRun", value: "process_run" },
                    { text: "ConnectionSession", value: "connection_session" },
                    { text: "CommandRequest", value: "command_request" },
                    { text: "Clock", value: "message_clock" },
                    { text: "Configuration", value: "message_configuration" },
                    { text: "Error", value: "message_error" },
                    { text: "Filter", value: "message_filter" },
                    { text: "Information", value: "message_information" },
                    { text: "Live", value: "message_live" },
                    { text: "OnzenLive", value: "message_onzen_live" },
                    { text: "OnzenSettings", value: "message_onzen_settings" },
                    { text: "Peak", value: "message_peak" },
                    { text: "Peripheral", value: "message_peripheral" },
                    { text: "Settings", value: "message_settings" }
                ]
                textRole: "text"
                valueRole: "value"
                onActivated: function(index) {
                    sqlModel.table = valueAt(index)
                    sqlTableView.positionViewAtCell(Qt.point(0, 0), TableView.AlignLeft | TableView.AlignTop)
                }
            }
        }

        RowLayout {
            Layout.topMargin: 5
            Layout.bottomMargin: 5
            Layout.leftMargin: 5
            Layout.rightMargin: 5

            spacing: 4

            TextField {
                id: genericFilterTextField
                placeholderText: "Generic Search"
                text: sqlModel.genericFilterString
                onTextChanged: sqlModel.genericFilterString = genericFilterTextField.text
                Keys.onReleased: function(event) {
                    if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
                        applyFilters()
                    }
                }
            }
            Button {
                text: "Search"
                onReleased: applyFilters()
            }
            Button {
                text: "Clear"
                onReleased: {
                    sqlModel.genericFilterString = ""
                    for (let i = 0; i < sqlModel.columnCount(); i++) {
                        sqlModel.setColumnFilterString(i, "");
                    }
                    applyFilters()
                }
            }
        }

        Rectangle {
            Layout.topMargin: 5
            Layout.bottomMargin: 0
            Layout.leftMargin: 0
            Layout.rightMargin: 0

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.horizontalStretchFactor: 1
            Layout.verticalStretchFactor: 1

            HorizontalHeaderView {
                id: sqlTableHeaderView

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top

                clip: true

                syncView: sqlTableView

                Menu {
                    id: sqlTableHeaderViewContextMenu

                    property int clickedColumn : 0

                    MenuItem {
                        text: "Size Column to Fit"
                        onTriggered: sqlTableView.setColumnWidthToFit(sqlTableHeaderViewContextMenu.clickedColumn)
                    }
                    MenuItem {
                        text: "Size All Columns to Fit"
                        onTriggered: sqlTableView.setAllColumnWidthsToFit()
                    }

                    MenuSeparator {}

                    ListModel {
                        id: columnVisibilityMenuModel
                    }
                    Menu {
                        id: columnVisibilityMenu
                        title: "Column Visibility"

                        Instantiator {
                            model: columnVisibilityMenuModel
                            delegate: MenuItem {
                                text: sqlModel.headerData(model.column, Qt.Horizontal)
                                checkable: true
                                checked: sqlTableView.columnWidth(model.column) !== 0
                                onTriggered: function() {
                                    if (sqlTableView.columnWidth(model.column) > 0) {
                                        sqlTableView.setColumnWidth(model.column, 0)
                                    } else {
                                        sqlTableView.setColumnWidthToFit(model.column)
                                    }
                                }
                            }
                            onObjectAdded: (index, object) => columnVisibilityMenu.insertItem(index, object)
                            onObjectRemoved: (index, object) => columnVisibilityMenu.removeItem(object)
                        }
                    }

                    Connections {
                        target: sqlModel
                        function onTableChanged() {
                            columnVisibilityMenuModel.clear()
                            for (let i = 0; i < sqlModel.columnCount(); i++) {
                                columnVisibilityMenuModel.append({ column: i })
                            }
                        }
                    }
                }

                delegate: Rectangle {
                    implicitHeight: delegateLayout.implicitHeight

                    readonly property string columnName : sqlModel.headerData(column, Qt.Horizontal)

                    ColumnLayout {
                        id: delegateLayout

                        anchors.fill: parent

                        RowLayout {
                            id: columnHeaderTextLayout

                            Layout.preferredHeight: 24
                            Layout.fillWidth: true
                            Layout.horizontalStretchFactor: 1

                            Rectangle {
                                Layout.fillHeight: true
                                width: 1
                                color: "#AAAAAA"
                            }

                            Rectangle {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                Layout.horizontalStretchFactor: 1
                                Layout.verticalStretchFactor: 1

                                Text {
                                    anchors.fill: parent
                                    // text: columnName
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                    text: {
                                        if (column === sqlModel.sortColumn) {
                                            // const sortSymbol = sqlModel.sortOrder === Qt.AscendingOrder ? "+" : "-"
                                            const sortSymbol = sqlModel.sortOrder === Qt.AscendingOrder ? "↑" : "↓"
                                            return `${columnName}  ${sortSymbol}`
                                        }
                                        return columnName
                                    }
                                    elide: Text.ElideRight
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    anchors.topMargin: 2
                                    anchors.bottomMargin: 2
                                    anchors.leftMargin: 5
                                    anchors.rightMargin: 5

                                    acceptedButtons: Qt.LeftButton | Qt.RightButton

                                    onReleased: function(mouse) {
                                        if (mouse.button === Qt.LeftButton) {
                                            let order = Qt.AscendingOrder
                                            if (column === sqlModel.sortColumn) {
                                                order = sqlModel.sortOrder === Qt.AscendingOrder ? Qt.DescendingOrder : Qt.AscendingOrder
                                            }
                                            sqlModel.applySort(column, order)
                                        } else if (mouse.button === Qt.RightButton) {
                                            sqlTableHeaderViewContextMenu.clickedColumn = column
                                            sqlTableHeaderViewContextMenu.popup()
                                        }
                                    }
                                }
                            }

                            Rectangle {
                                Layout.fillHeight: true
                                width: 1
                                color: "#AAAAAA"
                            }
                        }

                        TextField {
                            id: columnFilterTextField

                            Layout.preferredHeight: implicitHeight
                            Layout.fillWidth: true
                            Layout.horizontalStretchFactor: 1

                            text: sqlModel.columnFilterString(column)
                            onTextChanged: sqlModel.setColumnFilterString(column, columnFilterTextField.text)
                            Keys.onReleased: function(event) {
                                if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return) {
                                    applyFilters()
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                // edge-border
                id: sqlTableHeaderViewBorderBottom

                anchors.left: sqlTableHeaderView.left
                anchors.right: sqlTableHeaderView.right
                anchors.top: sqlTableHeaderView.bottom
                height: 1
                color: "#333333"
            }

            ScrollView {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: sqlTableHeaderViewBorderBottom.bottom
                anchors.bottom: parent.bottom

                TableView {
                    id: sqlTableView

                    clip: true

                    rowSpacing: 0
                    columnSpacing: 0

                    resizableColumns: true
                    boundsBehavior: Flickable.StopAtBounds

                    model: sqlModel

                    delegate: TableViewDelegate {
                        leftPadding: 5
                        rightPadding: 5
                        topPadding: 5
                        bottomPadding: 5
                    }

                    function minimumColumnWidth(column) {
                        const colWidthImplicit = implicitColumnWidth(column)
                        const colHeaderText = sqlModel.headerData(column, Qt.Horizontal)
                        const colHeaderWidthImplicit = (colHeaderText.length * 7) + 10
                        const colWidthMinimum = Math.max(colWidthImplicit, colHeaderWidthImplicit, 40)
                        return colWidthMinimum
                    }

                    function setColumnWidthToFit(column) {
                        setColumnWidth(column, minimumColumnWidth(column))
                    }
                    function setAllColumnWidthsToFit() {
                        for (let i = 0; i < sqlModel.columnCount(); i++) {
                            setColumnWidthToFit(i)
                        }
                    }

                    columnWidthProvider: function(column) {
                        const colWidthExplicit = explicitColumnWidth(column)
                        if (colWidthExplicit >= 0) {
                            return colWidthExplicit
                        }
                        return minimumColumnWidth(column)
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        sqlModel.table = tableComboBox.valueAt(tableComboBox.currentIndex)
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


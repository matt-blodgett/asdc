import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    property int currentPageIndex : 0

    StackLayout {
        anchors.fill: parent

        currentIndex: currentPageIndex

        DashboardPage {}
        ToolsPage {}
        SettingsPage {}        
    }

    Settings {
        category: "MainPage"
        property alias currentPageIndex: root.currentPageIndex
    }
}

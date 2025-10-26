import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    // Rectangle {
    //     id: tabBarBorderTop
    //     anchors.top: root.top
    //     width: root.width
    //     height: 1
    //     color: "#444444"
    // }

    // TabBar {
    //     id: tabBar
    //     anchors.top: tabBarBorderTop.bottom
    //     width: root.width

    //     TabButton {
    //         text: "Dashboard"
    //     }
    //     TabButton {
    //         text: "Settings"
    //     }
    //     TabButton {
    //         text: "Message Viewer"
    //     }
    //     TabButton {
    //         text: "SQL Viewer"
    //     }
    // }

    // Rectangle {
    //     id: tabBarBorderBottom
    //     anchors.top: tabBar.bottom
    //     width: root.width
    //     height: 1
    //     color: "#444444"
    // }

    // StackLayout {
    //     id: tabBarPagesLayout
    //     anchors.top: tabBarBorderBottom.bottom
    //     anchors.bottom: root.bottom
    //     width: root.width

    //     currentIndex: tabBar.currentIndex

    //     DashboardPage {}
    //     SettingsPage {}
    //     MessageViewerPage {}
    //     SqlBrowserPage {}
    // }

    property int currentPageIndex : 0

    StackLayout {
        id: tabBarPagesLayout
        anchors.top: root.top
        anchors.bottom: root.bottom
        width: root.width

        currentIndex: currentPageIndex

        DashboardPage {}
        MessageViewerPage {}
        SettingsPage {}
        // SqlBrowserPage {}
    }

    Settings {
        category: "MainPage"
        property alias currentPageIndex: root.currentPageIndex
    }
}

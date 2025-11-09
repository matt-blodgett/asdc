set_source_files_properties(AppStyle.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(asdc_qml
    URI src.qml
    VERSION 1.0
    QML_FILES
        # qml/components/FeatureControls.qml
        # qml/components/LoadingDialog.qml
        # qml/components/MessageViewer.qml
        # qml/components/SensorReadingsDisplay.qml
        # qml/components/TemperatureControls.qml

        # qml/pages/DashboardPage.qml
        # qml/pages/MainPage.qml
        # qml/pages/MessageViewerPage.qml
        # qml/pages/SettingsPage.qml
        # qml/pages/SqlBrowserPage.qml
        # qml/pages/StartupPage.qml
        # qml/pages/ToolsPage.qml

        AppStyle.qml
        Main.qml
)

# add_subdirectory(pages)
# add_subdirectory(components)

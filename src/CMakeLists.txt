set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Sql Quick Protobuf ProtobufQuick)
qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(asdc
    net/bytebuffer.h net/bytebuffer.cpp
    net/packet.h net/packet.cpp
    net/client.h net/client.cpp
    db/client.h db/client.cpp
    db/sqlquerymodel.h
    core.h core.cpp
    main.cpp
)

qt_add_qml_module(asdc
    URI asdc.qml
    VERSION 1.0
    QML_FILES
        qml/components/MessageViewer.qml
        qml/pages/DashboardPage.qml
        qml/pages/MainPage.qml
        qml/pages/MessageViewerPage.qml
        qml/pages/SettingsPage.qml
        qml/pages/SqlBrowserPage.qml
        qml/Main.qml
    RESOURCES
        assets/icons/check_circle.svg
        assets/icons/error_circle.svg
        assets/icons/progress_circle.svg
)

# # Resources:
# set(masterdetail_resource_files
#     "images/icon.png"
#     "images/image.png"
#     "albumdetails.xml"
# )

# qt_add_resources(masterdetail "masterdetail"
#     PREFIX
#         "/"
#     FILES
#         ${masterdetail_resource_files}
# )

qt_add_protobuf(asdc_proto
    QML
    QML_URI asdc.pb
    GENERATE_PACKAGE_SUBFOLDERS
    PROTO_FILES
        proto/Clock.proto
        proto/Command.proto
        proto/Configuration.proto
        proto/Error.proto
        proto/Filter.proto
        proto/Information.proto
        proto/Live.proto
        # proto/Lpc.proto
        # proto/MobileAuthenticate.proto
        # proto/MobileAvailableSpas.proto
        # proto/MobileSpaRegistration.proto
        # proto/MobileWifiDetails.proto
        proto/OnzenLive.proto
        proto/OnzenSettings.proto
        proto/Peak.proto
        proto/Peripheral.proto
        # proto/Router.proto
        proto/Settings.proto
)

target_link_libraries(asdc PRIVATE
    Qt6::Core
    Qt6::Sql
    Qt6::Quick
    Qt6::Protobuf
    Qt6::ProtobufQuick
    asdc_proto
)

set_target_properties(asdc PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appasdc_cpp_cmake
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS asdc
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# --------------------------------------------------------------------------------
# asdc - desktop

set(CMAKE_CXX_STANDARD_REQUIRED ON)


# --------------------------------------------------------------------------------
# qt setup

find_package(Qt6 REQUIRED COMPONENTS Core Sql Quick Protobuf ProtobufQuick)
qt_standard_project_setup(REQUIRES 6.8)


# --------------------------------------------------------------------------------
# buildinfo

set(BUILDINFO_VERSION ${CMAKE_PROJECT_VERSION})

string(TIMESTAMP BUILDINFO_DATE "%Y-%m-%d %H:%M:%S")

if (WIN32)
    set(BUILDINFO_OS "Windows")
elseif (APPLE)
    set(BUILDINFO_OS "macOS")
elseif (UNIX)
    set(BUILDINFO_OS "Linux")
else()
    set(BUILDINFO_OS "Unknown")
endif()

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(BUILDINFO_ARCH "x86_64")
else()
    set(BUILDINFO_ARCH "x86")
endif()

set(BUILDINFO_COMPILER "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

set(BUILDINFO_QT_VERSION "${Qt6Core_VERSION}")

configure_file(
    ${CMAKE_SOURCE_DIR}/src/buildinfo.h.in
    ${CMAKE_BINARY_DIR}/src/buildinfo.h
    @ONLY
)


# --------------------------------------------------------------------------------
# executable

qt_add_executable(asdc
    net/bytebuffer.h net/bytebuffer.cpp
    net/packet.h net/packet.cpp
    net/client.h net/client.cpp
    db/client.h db/client.cpp

    net/discovery.h net/discovery.cpp
    db/sqlquerymodel.h

    appbuildinfo.h

    core.h core.cpp
    main.cpp
)


# --------------------------------------------------------------------------------
# qml

set_source_files_properties(qml/AppStyle.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(asdc
    URI src.qml
    VERSION 1.0
    QML_FILES
        qml/components/AboutDialog.qml
        qml/components/FeatureControls.qml
        qml/components/LoadingDialog.qml
        qml/components/MessageViewer.qml
        qml/components/SensorReadingsDisplay.qml
        qml/components/TemperatureControls.qml
        qml/components/TemperatureDial.qml

        qml/pages/DashboardPage.qml
        qml/pages/MainPage.qml
        qml/pages/MessageViewerPage.qml
        qml/pages/SettingsPage.qml
        qml/pages/SqlBrowserPage.qml
        qml/pages/StartupPage.qml
        qml/pages/ToolsPage.qml

        qml/AppStyle.qml
        qml/Main.qml
)


# --------------------------------------------------------------------------------
# resources

qt_add_resources(assets qml/assets/assets.qrc)
target_sources(asdc PRIVATE ${assets})

if(WIN32)
    target_sources(asdc PRIVATE qml/assets/win32.rc)
endif()


# --------------------------------------------------------------------------------
# protobuf

qt_add_protobuf(asdc_qml_proto
    QML
    QML_URI src.qml.proto
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


# --------------------------------------------------------------------------------
# target / install

target_link_libraries(asdc PRIVATE
    Qt6::Core
    Qt6::Sql
    Qt6::Quick
    Qt6::Protobuf
    Qt6::ProtobufQuick

    asdc_qml_proto
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

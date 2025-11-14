#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
// #include <QIcon>
// #include <QStyleHints>

#include <QLoggingCategory>

#include "appbuildinfo.h"
#include "core.h"
#include "db/sqlquerymodel.h"


// #define MESSAGE_PATTERN "%{time yyyy-MM-dd hh:mm:ss.zzz} %{if-debug}DEBUG%{endif}%{if-info}INFO%{endif}%{if-warning}WARNING%{endif}%{if-critical}CRITICAL%{endif}%{if-fatal}FATAL%{endif} %{category} %{file}:%{line} <%{function}> %{message}"
// #define MESSAGE_PATTERN "%{time yyyy-MM-dd hh:mm:ss.zzz} %{if-debug}DEBUG%{endif}%{if-info}INFO%{endif}%{if-warning}WARNING%{endif}%{if-critical}CRITICAL%{endif}%{if-fatal}FATAL%{endif} %{category} <%{function}> %{message}"

// #define MESSAGE_PATTERN "%{time yyyy-MM-ddThh:mm:ss.zzz} | %{if-debug}DEBUG%{endif}%{if-info}INFO%{endif}%{if-warning}WARNING%{endif}%{if-critical}CRITICAL%{endif}%{if-fatal}FATAL%{endif} | %{category} | %{function} | %{file}:%{line} | %{message}"
#define MESSAGE_PATTERN "%{time yyyy-MM-ddThh:mm:ss.zzz} | %{if-debug}DEBUG%{endif}%{if-info}INFO%{endif}%{if-warning}WARNING%{endif}%{if-critical}CRITICAL%{endif}%{if-fatal}FATAL%{endif} | %{category} | %{function} | %{message}"
// #define MESSAGE_PATTERN "%{time yyyy-MM-ddThh:mm:ss.zzz} | %{if-debug}DEBUG%{endif}%{if-info}INFO%{endif}%{if-warning}WARNING%{endif}%{if-critical}CRITICAL%{endif}%{if-fatal}FATAL%{endif} | %{category} | %{message}"


Q_LOGGING_CATEGORY(coreLog, "asdc.core")
Q_LOGGING_CATEGORY(netLog, "asdc.net")
Q_LOGGING_CATEGORY(dbLog, "asdc.db")
Q_LOGGING_CATEGORY(workerLog, "asdc.worker")



void initLogging() {
    qSetMessagePattern(MESSAGE_PATTERN);

    QStringList filterRules;

    // filterRules << "asdc.*.debug=false";

    filterRules << "asdc.core.debug=true";
    filterRules << "asdc.core.info=true";

    filterRules << "asdc.net.debug=false";
    filterRules << "asdc.net.info=false";

    filterRules << "asdc.db.debug=false";
    filterRules << "asdc.db.info=true";

    filterRules << "asdc.worker.debug=false";
    filterRules << "asdc.worker.info=false";

    filterRules << "qml.debug=true";

    filterRules << "default.debug=true";

    QString filterRulesString = filterRules.join("\n");
    QLoggingCategory::setFilterRules(filterRulesString);
}

int main(int argc, char *argv[])
{
    initLogging();

    // qDebug() << "Version:" << BUILDINFO_VERSION;
    // qDebug() << "Build Date:" << BUILDINFO_DATE;
    // qDebug() << "Build Type:" << BUILDINFO_TYPE;
    // qDebug() << "OS:" << BUILDINFO_OS;
    // qDebug() << "Arch:" << BUILDINFO_ARCH;
    // qDebug() << "Compiler:" << BUILDINFO_COMPILER;
    // qDebug() << "Qt Version:" << BUILDINFO_QT_VERSION;

    QCoreApplication::setOrganizationName("QtProject");
    QCoreApplication::setApplicationName("asdc");
    QCoreApplication::setApplicationVersion(BUILDINFO_VERSION);

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Light);
    // app.setWindowIcon(QIcon("src/assets/icons/hot_tub.ico"));

    qRegisterMetaType<QAbstractSocket::SocketState>();
    qmlRegisterUncreatableType<QAbstractSocket>("asdc.types.sockets", 1, 0, "SocketState", "SocketState is an enum, not a creatable type.");

    qmlRegisterType<SqlTableModel>("asdc.types.models", 1, 0, "SqlTableModel");
    qmlRegisterType<SqlQueryModel>("asdc.types.models", 1, 0, "SqlQueryModel");

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    QObject::connect(&engine, &QQmlApplicationEngine::quit, &app, &QGuiApplication::quit, Qt::QueuedConnection);

    AppBuildInfo *appBuildInfo = new AppBuildInfo(&app);
    engine.rootContext()->setContextProperty("appBuildInfo", appBuildInfo);

    asdc::core::CoreInterface *core = new asdc::core::CoreInterface(&app);
    engine.rootContext()->setContextProperty("core", core);

    // qDebug() << engine.importPathList();
    // qDebug() << engine.baseUrl();

    engine.loadFromModule("src.qml", "Main");

    return app.exec();
}

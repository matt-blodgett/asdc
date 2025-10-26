#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <qlogging.h>
#include <QIcon>

#include "core.h"

#include "db/sqlquerymodel.h"


//#define MESSAGE_PATTERN "[%{time yyyyMMdd h:mm:ss.zzz}] %{if-debug}DBUG:%{endif}%{if-info}INFO:%{endif}%{if-warning}WARN:%{endif}%{if-critical}CRIT:%{endif}%{if-fatal}FATAL:%{endif} <%{function}> %{message}"
#define MESSAGE_PATTERN "%{if-debug}DBUG:%{endif}%{if-info}INFO:%{endif}%{if-warning}WARN:%{endif}%{if-critical}CRIT:%{endif}%{if-fatal}FATAL:%{endif} <%{function}> %{message}"





int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("QtProject");
    QCoreApplication::setApplicationName("asdc");
    QCoreApplication::setApplicationVersion("0.1.0");

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // app.setWindowIcon(QIcon("src/assets/icons/hot_tub.ico"));

    qSetMessagePattern(MESSAGE_PATTERN);

    qRegisterMetaType<QAbstractSocket::SocketState>();
    qmlRegisterUncreatableType<QAbstractSocket>("asdc.types.sockets", 1, 0, "SocketState", "SocketState is an enum, not a creatable type.");

    qmlRegisterType<SqlTableModel>("asdc.types.models", 1, 0, "SqlTableModel");
    qmlRegisterType<SqlQueryModel>("asdc.types.models", 1, 0, "SqlQueryModel");

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
    QObject::connect(&engine, &QQmlApplicationEngine::quit, &app, &QGuiApplication::quit, Qt::QueuedConnection);

    asdc::core::CoreInterface *core = new asdc::core::CoreInterface(&app);
    engine.rootContext()->setContextProperty("core", core);

    engine.loadFromModule("asdc.qml", "Main");

    return app.exec();
}

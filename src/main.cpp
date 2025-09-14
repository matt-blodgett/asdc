#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <qlogging.h>


#include "core.h"


//#define MESSAGE_PATTERN "[%{time yyyyMMdd h:mm:ss.zzz}] %{if-debug}DBUG:%{endif}%{if-info}INFO:%{endif}%{if-warning}WARN:%{endif}%{if-critical}CRIT:%{endif}%{if-fatal}FATAL:%{endif} <%{function}> %{message}"
#define MESSAGE_PATTERN "%{if-debug}DBUG:%{endif}%{if-info}INFO:%{endif}%{if-warning}WARN:%{endif}%{if-critical}CRIT:%{endif}%{if-fatal}FATAL:%{endif} <%{function}> %{message}"


int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("QtProject");
    QCoreApplication::setApplicationName("asdc");
    QCoreApplication::setApplicationVersion("0.1.0");

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qSetMessagePattern(MESSAGE_PATTERN);

    qRegisterMetaType<QAbstractSocket::SocketState>();
    qmlRegisterUncreatableType<QAbstractSocket>("asdc.types.sockets", 1, 0, "SocketState", "SocketState is an enum, not a creatable type.");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection
    );
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::quit,
        &app,
        &QGuiApplication::quit,
        Qt::QueuedConnection
    );

    asdc::core::Core *core = new asdc::core::Core(&app);
    engine.rootContext()->setContextProperty("core", core);
    engine.loadFromModule("asdc.qml", "Main");

    return app.exec();
}

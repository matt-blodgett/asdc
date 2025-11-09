#ifndef DB_CLIENT_H
#define DB_CLIENT_H

#include <QObject>
#include <QSqlDatabase>

QT_FORWARD_DECLARE_CLASS(QProtobufMessage)

#include "asdc/proto/Clock.qpb.h"
#include "asdc/proto/Configuration.qpb.h"
#include "asdc/proto/Error.qpb.h"
#include "asdc/proto/Filter.qpb.h"
#include "asdc/proto/Information.qpb.h"
#include "asdc/proto/Live.qpb.h"
#include "asdc/proto/OnzenLive.qpb.h"
#include "asdc/proto/OnzenSettings.qpb.h"
#include "asdc/proto/Peak.qpb.h"
#include "asdc/proto/Peripheral.qpb.h"
#include "asdc/proto/Settings.qpb.h"


namespace asdc::net {
enum class MessageType;
}

namespace asdc::db {


class DatabaseClient : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseClient(QObject *parent = nullptr);

public:
    bool openDatabase(const bool &overwrite = false);

private:
    bool validateSchema();
    void initializeSchema();

    void createProcessRun();

public:
    void createConnectionSession(const QString &host);

    void logMessageClock(const asdc::proto::Clock &message, const QDateTime &messageReceivedAt);
    void logMessageConfiguration(const asdc::proto::Configuration &message, const QDateTime &messageReceivedAt);
    void logMessageError(const asdc::proto::Error &message, const QDateTime &messageReceivedAt);
    void logMessageFilter(const asdc::proto::Filter &message, const QDateTime &messageReceivedAt);
    void logMessageInformation(const asdc::proto::Information &message, const QDateTime &messageReceivedAt);
    void logMessageLive(const asdc::proto::Live &message, const QDateTime &messageReceivedAt);
    void logMessageOnzenLive(const asdc::proto::OnzenLive &message, const QDateTime &messageReceivedAt);
    void logMessageOnzenSettings(const asdc::proto::OnzenSettings &message, const QDateTime &messageReceivedAt);
    void logMessagePeak(const asdc::proto::Peak &message, const QDateTime &messageReceivedAt);
    void logMessagePeripheral(const asdc::proto::Peripheral &message, const QDateTime &messageReceivedAt);
    void logMessageSettings(const asdc::proto::Settings &message, const QDateTime &messageReceivedAt);

    void logCommand(const QString &name, const QVariant &valueFrom, const QVariant &valueTo, const QDateTime &commandSentAt);

private:
    QSqlDatabase m_database;
    QVariant m_processRunId;
    QVariant m_connectionSessionId;

signals:
};

};  // namespace asdc::db

#endif // DB_CLIENT_H

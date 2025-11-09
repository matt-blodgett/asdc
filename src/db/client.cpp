#include "client.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QDateTime>
#include <QVariant>
#include <QMap>
#include <QFile>

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(dbLog)


// #include "asdc/proto/Clock.qpb.h"


// const auto ASDC_DATABASE_NAME = QLatin1String("asdc.db");
const auto ASDC_DATABASE_NAME = QLatin1String("C:/Users/matt/Desktop/asdc.db");


const auto ASDC_SQL_CREATE_TABLE_PROCESS_RUN = QLatin1String(R"(
    CREATE TABLE "process_run" (
        "id" INTEGER PRIMARY KEY,
        "created_at" DATETIME
    )
)");
const auto ASDC_SQL_CREATE_TABLE_CONNECTION_SESSION = QLatin1String(R"(
    CREATE TABLE "connection_session" (
        "id" INTEGER PRIMARY KEY,
        "created_at" DATETIME,
        "process_run_id" INTEGER,

        "host" TEXT,

        FOREIGN KEY ("process_run_id")
            REFERENCES "process_run" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE
    )
)");
const auto ASDC_SQL_CREATE_TABLE_MESSAGE_CLOCK = QLatin1String(R"(
    CREATE TABLE "message_clock" (
        "id" INTEGER PRIMARY KEY,
        "created_at" DATETIME,
        "process_run_id" INTEGER,
        "connection_session_id" INTEGER,
        "message_received_at" DATETIME,

        "year" INTEGER,
        "month" INTEGER,
        "day" INTEGER,
        "hour" INTEGER,
        "minute" INTEGER,
        "second" INTEGER,

        FOREIGN KEY ("process_run_id")
            REFERENCES "process_run" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE

        FOREIGN KEY ("connection_session_id")
            REFERENCES "connection_session" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE
    )
)");
const auto ASDC_SQL_CREATE_TABLE_MESSAGE_CONFIGURATION = QLatin1String(R"(
    CREATE TABLE "message_configuration" (
        "id" INTEGER PRIMARY KEY,
        "created_at" DATETIME,
        "process_run_id" INTEGER,
        "connection_session_id" INTEGER,
        "message_received_at" DATETIME,

        "pump1" BOOLEAN,
        "pump2" BOOLEAN,
        "pump3" BOOLEAN,
        "pump4" BOOLEAN,
        "pump5" BOOLEAN,
        "blower1" BOOLEAN,
        "blower2" BOOLEAN,
        "lights" BOOLEAN,
        "stereo" BOOLEAN,
        "heater1" BOOLEAN,
        "heater2" BOOLEAN,
        "filter" BOOLEAN,
        "onzen" BOOLEAN,
        "ozone_peak_1" BOOLEAN,
        "ozone_peak_2" BOOLEAN,
        "exhaust_fan" BOOLEAN,
        "powerlines" INTEGER,
        "breaker_size" INTEGER,
        "smart_onzen" INTEGER,
        "fogger" BOOLEAN,
        "sds" BOOLEAN,
        "yess" BOOLEAN,

        FOREIGN KEY ("process_run_id")
            REFERENCES "process_run" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE

        FOREIGN KEY ("connection_session_id")
            REFERENCES "connection_session" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE
    )
)");
const auto ASDC_SQL_CREATE_TABLE_MESSAGE_ERROR = QLatin1String(R"(
    CREATE TABLE "message_error" (
        "id" INTEGER PRIMARY KEY,
        "created_at" DATETIME,
        "process_run_id" INTEGER,
        "connection_session_id" INTEGER,
        "message_received_at" DATETIME,

        "no_flow" BOOLEAN,
        "flow_switch" BOOLEAN,
        "heater_over_temperature" BOOLEAN,
        "spa_over_temperature" BOOLEAN,
        "spa_temperature_probe" BOOLEAN,
        "spa_high_limit" BOOLEAN,
        "eeprom" BOOLEAN,
        "freeze_protect" BOOLEAN,
        "ph_high" BOOLEAN,
        "heater_probe_disconnected" BOOLEAN,

        FOREIGN KEY ("process_run_id")
            REFERENCES "process_run" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE

        FOREIGN KEY ("connection_session_id")
            REFERENCES "connection_session" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE
    )
)");
const auto ASDC_SQL_CREATE_TABLE_MESSAGE_FILTER = QLatin1String(R"(
    CREATE TABLE "message_filter" (
        "id" INTEGER PRIMARY KEY,
        "created_at" DATETIME,
        "process_run_id" INTEGER,
        "connection_session_id" INTEGER,
        "message_received_at" DATETIME,

        "serial_nums" TEXT,
        "filter_state" INTEGER,
        "install_dates" TEXT,

        FOREIGN KEY ("process_run_id")
            REFERENCES "process_run" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE

        FOREIGN KEY ("connection_session_id")
            REFERENCES "connection_session" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE
    )
)");
const auto ASDC_SQL_CREATE_TABLE_MESSAGE_INFORMATION = QLatin1String(R"(
    CREATE TABLE "message_information" (
        "id" INTEGER PRIMARY KEY,
        "created_at" DATETIME,
        "process_run_id" INTEGER,
        "connection_session_id" INTEGER,
        "message_received_at" DATETIME,

        "pack_serial_number" TEXT,
        "pack_firmware_version" TEXT,
        "pack_hardware_version" TEXT,
        "pack_product_id" TEXT,
        "pack_board_id" TEXT,
        "topside_product_id" TEXT,
        "topside_software_version" TEXT,
        "guid" TEXT,
        "spa_type" INTEGER,
        "website_registration" BOOLEAN,
        "website_registration_confirm" BOOLEAN,
        "mac_address" BLOB,
        "firmware_version" INTEGER,
        "product_code" INTEGER,
        "var_software_version" TEXT,
        "spaboy_firmware_version" TEXT,
        "spaboy_hardware_version" TEXT,
        "spaboy_product_id" TEXT,
        "spaboy_serial_number" TEXT,
        "rfid_firmware_version" TEXT,
        "rfid_hardware_version" TEXT,
        "rfid_product_id" TEXT,
        "rfid_serial_number" TEXT,

        FOREIGN KEY ("process_run_id")
            REFERENCES "process_run" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE

        FOREIGN KEY ("connection_session_id")
            REFERENCES "connection_session" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE
    )
)");
const auto ASDC_SQL_CREATE_TABLE_MESSAGE_LIVE = QLatin1String(R"(
    CREATE TABLE "message_live" (
        "id" INTEGER PRIMARY KEY,
        "created_at" DATETIME,
        "process_run_id" INTEGER,
        "connection_session_id" INTEGER,
        "message_received_at" DATETIME,

        "temperature_fahrenheit" INTEGER,
        "temperature_setpoint_fahrenheit" INTEGER,
        "pump_1" INTEGER,
        "pump_2" INTEGER,
        "pump_3" INTEGER,
        "pump_4" INTEGER,
        "pump_5" INTEGER,
        "blower_1" INTEGER,
        "blower_2" INTEGER,
        "lights" BOOLEAN,
        "stereo" BOOLEAN,
        "heater_1" INTEGER,
        "heater_2" INTEGER,
        "filter" INTEGER,
        "onzen" BOOLEAN,
        "ozone" INTEGER,
        "exhaust_fan" BOOLEAN,
        "sauna" INTEGER,
        "heater_adc" INTEGER,
        "sauna_time_remaining" INTEGER,
        "economy" BOOLEAN,
        "current_adc" INTEGER,
        "all_on" BOOLEAN,
        "fogger" BOOLEAN,
        "error" INTEGER,
        "alarm" INTEGER,
        "status" INTEGER,
        "ph" INTEGER,
        "orp" INTEGER,
        "sds" BOOLEAN,
        "yess" BOOLEAN,

        FOREIGN KEY ("process_run_id")
            REFERENCES "process_run" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE

        FOREIGN KEY ("connection_session_id")
            REFERENCES "connection_session" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE
    )
)");
const auto ASDC_SQL_CREATE_TABLE_MESSAGE_ONZEN_LIVE = QLatin1String(R"(
    CREATE TABLE "message_onzen_live" (
        "id" INTEGER PRIMARY KEY,
        "created_at" DATETIME,
        "process_run_id" INTEGER,
        "connection_session_id" INTEGER,
        "message_received_at" DATETIME,

        "guid" TEXT,
        "orp" INTEGER,
        "ph_100" INTEGER,
        "current" INTEGER,
        "voltage" INTEGER,
        "current_setpoint" INTEGER,
        "voltage_setpoint" INTEGER,
        "pump1" BOOLEAN,
        "pump2" BOOLEAN,
        "orp_state_machine" INTEGER,
        "electrode_state_machine" INTEGER,
        "electrode_id" INTEGER,
        "electrode_polarity" INTEGER,
        "electrode_1_resistance_1" INTEGER,
        "electrode_1_resistance_2" INTEGER,
        "electrode_2_resistance_1" INTEGER,
        "electrode_2_resistance_2" INTEGER,
        "command_mode" BOOLEAN,
        "electrode_mah" INTEGER,
        "ph_color" INTEGER,
        "orp_color" INTEGER,
        "electrode_wear" INTEGER,

        FOREIGN KEY ("process_run_id")
            REFERENCES "process_run" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE

        FOREIGN KEY ("connection_session_id")
            REFERENCES "connection_session" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE
    )
)");
const auto ASDC_SQL_CREATE_TABLE_MESSAGE_ONZEN_SETTINGS = QLatin1String(R"(
    CREATE TABLE "message_onzen_settings" (
        "id" INTEGER PRIMARY KEY,
        "created_at" DATETIME,
        "process_run_id" INTEGER,
        "connection_session_id" INTEGER,
        "message_received_at" DATETIME,

        "guid" TEXT,
        "over_voltage" INTEGER,
        "under_voltage" INTEGER,
        "over_current" INTEGER,
        "under_current" INTEGER,
        "orp_high" INTEGER,
        "orp_low" INTEGER,
        "ph_high" INTEGER,
        "ph_low" INTEGER,
        "pwm_pump1_time_on" INTEGER,
        "pwm_pump1_time_off" INTEGER,
        "sampling_interval" INTEGER,
        "sampling_duration" INTEGER,
        "pwm_pump2_time_on" INTEGER,
        "pwm_pump2_time_off" INTEGER,
        "sb_low_cl" INTEGER,
        "sb_caution_low_cl" INTEGER,
        "sb_caution_high_cl" INTEGER,
        "sb_high_cl" INTEGER,
        "sb_low_ph" INTEGER,
        "sb_caution_low_ph" INTEGER,
        "sb_caution_high_ph" INTEGER,
        "sb_high_ph" INTEGER,

        FOREIGN KEY ("process_run_id")
            REFERENCES "process_run" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE

        FOREIGN KEY ("connection_session_id")
            REFERENCES "connection_session" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE
    )
)");
const auto ASDC_SQL_CREATE_TABLE_MESSAGE_PEAK = QLatin1String(R"(
    CREATE TABLE "message_peak" (
        "id" INTEGER PRIMARY KEY,
        "created_at" DATETIME,
        "process_run_id" INTEGER,
        "connection_session_id" INTEGER,
        "message_received_at" DATETIME,

        "peaknum" INTEGER,
        "peakstart1" INTEGER,
        "peakend1" INTEGER,
        "peakstart2" INTEGER,
        "peakend2" INTEGER,
        "midpeaknum" INTEGER,
        "midpeakstart1" INTEGER,
        "midpeakend1" INTEGER,
        "midpeakstart2" INTEGER,
        "midpeakend2" INTEGER,
        "offpeakstart" INTEGER,
        "offpeakend" INTEGER,
        "offset" INTEGER,
        "peakheater" BOOLEAN,
        "peakfilter" BOOLEAN,
        "peakozone" BOOLEAN,
        "midpeakheater" BOOLEAN,
        "midpeakfilter" BOOLEAN,
        "midpeakozone" BOOLEAN,
        "sat" BOOLEAN,
        "sun" BOOLEAN,
        "mon" BOOLEAN,
        "tue" BOOLEAN,
        "wed" BOOLEAN,
        "thu" BOOLEAN,
        "fri" BOOLEAN,

        FOREIGN KEY ("process_run_id")
            REFERENCES "process_run" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE

        FOREIGN KEY ("connection_session_id")
            REFERENCES "connection_session" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE
    )
)");
const auto ASDC_SQL_CREATE_TABLE_MESSAGE_PERIPHERAL = QLatin1String(R"(
    CREATE TABLE "message_peripheral" (
        "id" INTEGER PRIMARY KEY,
        "created_at" DATETIME,
        "process_run_id" INTEGER,
        "connection_session_id" INTEGER,
        "message_received_at" DATETIME,

        "guid" TEXT,
        "hardware_version" INTEGER,
        "firmware_version" INTEGER,
        "product_code" INTEGER,
        "connected" BOOLEAN,

        FOREIGN KEY ("process_run_id")
            REFERENCES "process_run" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE

        FOREIGN KEY ("connection_session_id")
            REFERENCES "connection_session" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE
    )
)");
const auto ASDC_SQL_CREATE_TABLE_MESSAGE_SETTINGS = QLatin1String(R"(
    CREATE TABLE "message_settings" (
        "id" INTEGER PRIMARY KEY,
        "created_at" DATETIME,
        "process_run_id" INTEGER,
        "connection_session_id" INTEGER,
        "message_received_at" DATETIME,

        "max_filtration_frequency" INTEGER,
        "min_filtration_frequency" INTEGER,
        "filtration_frequency" INTEGER,
        "max_filtration_duration" INTEGER,
        "min_filtration_duration" INTEGER,
        "filtration_duration" INTEGER,
        "max_onzen_hours" INTEGER,
        "min_onzen_hours" INTEGER,
        "onzen_hours" INTEGER,
        "max_onzen_cycles" INTEGER,
        "min_onzen_cycles" INTEGER,
        "onzen_cycles" INTEGER,
        "max_ozone_hours" INTEGER,
        "min_ozone_hours" INTEGER,
        "ozone_hours" INTEGER,
        "max_ozone_cycles" INTEGER,
        "min_ozone_cycles" INTEGER,
        "ozone_cycles" INTEGER,
        "filter_suspension" BOOLEAN,
        "flash_lights_on_error" BOOLEAN,
        "temperature_offset" INTEGER,
        "sauna_duration" INTEGER,
        "min_temperature" INTEGER,
        "max_temperature" INTEGER,
        "filtration_offset" INTEGER,
        "spaboy_hours" INTEGER,

        FOREIGN KEY ("process_run_id")
            REFERENCES "process_run" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE

        FOREIGN KEY ("connection_session_id")
            REFERENCES "connection_session" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE
    )
)");

const auto ASDC_SQL_CREATE_TABLE_COMMAND_REQUEST = QLatin1String(R"(
    CREATE TABLE "command_request" (
        "id" INTEGER PRIMARY KEY,
        "created_at" DATETIME,
        "process_run_id" INTEGER,
        "connection_session_id" INTEGER,
        "command_sent_at" DATETIME,

        "name" TEXT,
        "value_from" TEXT,
        "value_to" TEXT,

        FOREIGN KEY ("process_run_id")
            REFERENCES "process_run" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE

        FOREIGN KEY ("connection_session_id")
            REFERENCES "connection_session" ("id")
            ON DELETE CASCADE
            ON UPDATE CASCADE
    )
)");


// static const QMap<QString, QMetaType::Type> messageFieldsClock() {
//     QMap<QString, QMetaType::Type> messageFields = {
//         {"year", QMetaType::Int},
//         {"month", QMetaType::Int},
//         {"day", QMetaType::Int},
//         {"hour", QMetaType::Int},
//         {"minute", QMetaType::Int},
//         {"second", QMetaType::Int}
//     };
//     return messageFields;
// }


// #include "asdc/proto/Clock.qpb.h"
// #include "asdc/proto/Configuration.qpb.h"
// #include "asdc/proto/Error.qpb.h"
// #include "asdc/proto/Filter.qpb.h"
// #include "asdc/proto/Information.qpb.h"
// #include "asdc/proto/Live.qpb.h"
// #include "asdc/proto/OnzenLive.qpb.h"
// #include "asdc/proto/OnzenSettings.qpb.h"
// #include "asdc/proto/Peak.qpb.h"
// #include "asdc/proto/Peripheral.qpb.h"
// #include "asdc/proto/Settings.qpb.h"


namespace asdc::db {


DatabaseClient::DatabaseClient(QObject *parent)
    : QObject{parent}
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");
}

bool DatabaseClient::openDatabase(const bool &overwrite) {
    const QString path(ASDC_DATABASE_NAME);

    qCInfo(dbLog) << "connecting to database" << path;

    QFile databaseFile(path);

    const bool isExistingDatabaseFile = databaseFile.exists();

    if (isExistingDatabaseFile && overwrite) {
        qCWarning(dbLog) << "overwrite enabled, removing existing database file" << path;
        if (databaseFile.remove()) {
            qCDebug(dbLog) << "successfully deleted file" << path;
        } else {
            qCCritical(dbLog) << "error deleting existing database file" << path;
            qCCritical(dbLog) << "error details:" << databaseFile.errorString();
            return false;
        }
    }

    m_database.setDatabaseName(path);

    if (!m_database.open()) {
        qCCritical(dbLog) << "error opening database file" << path;
        qCCritical(dbLog) << "error details:" << m_database.lastError();
        return false;
    }

    if (isExistingDatabaseFile && !overwrite) {
        if (!validateSchema()) {
            return false;
        }
    } else {
        initializeSchema();
    }

    createProcessRun();

    qCInfo(dbLog) << "successfully setup database connection" << path;

    return true;
}

bool DatabaseClient::validateSchema() {
    qCDebug(dbLog) << "validating schema";

    const QStringList tables = {
        "process_run",
        "connection_session",
        "message_clock"
    };

    for (const QString &table : tables) {
        if (!m_database.tables().contains(table)) {
            qCCritical(dbLog) << "database is improperly configured or corrupt: does not contain table" << table;
            return false;
        }
    }

    qCDebug(dbLog) << "all schema validation tests passed";

    return true;
}
void DatabaseClient::initializeSchema() {
    qCDebug(dbLog) << "initializing schema";

    QSqlQuery query(m_database);

    query.exec(ASDC_SQL_CREATE_TABLE_PROCESS_RUN);
    query.exec(ASDC_SQL_CREATE_TABLE_CONNECTION_SESSION);

    query.exec(ASDC_SQL_CREATE_TABLE_MESSAGE_CLOCK);
    query.exec(ASDC_SQL_CREATE_TABLE_MESSAGE_CONFIGURATION);
    query.exec(ASDC_SQL_CREATE_TABLE_MESSAGE_ERROR);
    query.exec(ASDC_SQL_CREATE_TABLE_MESSAGE_FILTER);
    query.exec(ASDC_SQL_CREATE_TABLE_MESSAGE_INFORMATION);
    query.exec(ASDC_SQL_CREATE_TABLE_MESSAGE_LIVE);
    query.exec(ASDC_SQL_CREATE_TABLE_MESSAGE_ONZEN_LIVE);
    query.exec(ASDC_SQL_CREATE_TABLE_MESSAGE_ONZEN_SETTINGS);
    query.exec(ASDC_SQL_CREATE_TABLE_MESSAGE_PEAK);
    query.exec(ASDC_SQL_CREATE_TABLE_MESSAGE_PERIPHERAL);
    query.exec(ASDC_SQL_CREATE_TABLE_MESSAGE_SETTINGS);

    query.exec(ASDC_SQL_CREATE_TABLE_COMMAND_REQUEST);

    qCDebug(dbLog) << "successfully initialized schema";
}

void DatabaseClient::createProcessRun() {
    QSqlQuery query(m_database);

    query.exec(QLatin1String(R"(
        INSERT INTO "process_run" (
           "created_at"
        )
        VALUES (DATETIME('now'))
    )"));

    m_processRunId = query.lastInsertId();

    qCDebug(dbLog) << "created processRunId" << m_processRunId.toLongLong();
}
void DatabaseClient::createConnectionSession(const QString &host) {
    QSqlQuery query(m_database);

    query.prepare(QLatin1String(R"(
        INSERT INTO "connection_session" (
            "created_at",
            "process_run_id",

            "host"
        )
        VALUES (DATETIME('now'), ?, ?)
    )"));

    int pos = 0;
    query.bindValue(pos++, m_processRunId);
    query.bindValue(pos++, host);
    query.exec();

    m_connectionSessionId = query.lastInsertId();

    qCDebug(dbLog) << "created connectionSessionId" << m_connectionSessionId.toLongLong();
}

void DatabaseClient::logMessageClock(const asdc::proto::Clock &message, const QDateTime &messageReceivedAt) {
    QSqlQuery query(m_database);

    query.prepare(QLatin1String(R"(
        INSERT INTO "message_clock" (
            "created_at",
            "process_run_id",
            "connection_session_id",
            "message_received_at",

            "year",
            "month",
            "day",
            "hour",
            "minute",
            "second"
        )
        VALUES (DATETIME('now'), ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )"));

    int pos = 0;

    query.bindValue(pos++, m_processRunId);
    query.bindValue(pos++, m_connectionSessionId);
    query.bindValue(pos++, messageReceivedAt.toString("yyyy-MM-dd hh:mm:ss"));

    query.bindValue(pos++, message.property("year").toInt());
    query.bindValue(pos++, message.property("month").toInt());
    query.bindValue(pos++, message.property("day").toInt());
    query.bindValue(pos++, message.property("hour").toInt());
    query.bindValue(pos++, message.property("minute").toInt());
    query.bindValue(pos++, message.property("second").toInt());

    query.exec();

    qCDebug(dbLog) << "wrote message" << message.staticMetaObject.className();
}
void DatabaseClient::logMessageConfiguration(const asdc::proto::Configuration &message, const QDateTime &messageReceivedAt) {
    QSqlQuery query(m_database);

    query.prepare(QLatin1String(R"(
        INSERT INTO "message_configuration" (
            "created_at",
            "process_run_id",
            "connection_session_id",
            "message_received_at",

            "pump1",
            "pump2",
            "pump3",
            "pump4",
            "pump5",
            "blower1",
            "blower2",
            "lights",
            "stereo",
            "heater1",
            "heater2",
            "filter",
            "onzen",
            "ozone_peak_1",
            "ozone_peak_2",
            "exhaust_fan",
            "powerlines",
            "breaker_size",
            "smart_onzen",
            "fogger",
            "sds",
            "yess"
        )
        VALUES (DATETIME('now'), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )"));

    int pos = 0;

    query.bindValue(pos++, m_processRunId);
    query.bindValue(pos++, m_connectionSessionId);
    query.bindValue(pos++, messageReceivedAt.toString("yyyy-MM-dd hh:mm:ss"));

    query.bindValue(pos++, message.property("pump1").toBool());
    query.bindValue(pos++, message.property("pump2").toBool());
    query.bindValue(pos++, message.property("pump3").toBool());
    query.bindValue(pos++, message.property("pump4").toBool());
    query.bindValue(pos++, message.property("pump5").toBool());
    query.bindValue(pos++, message.property("blower1").toBool());
    query.bindValue(pos++, message.property("blower2").toBool());
    query.bindValue(pos++, message.property("lights").toBool());
    query.bindValue(pos++, message.property("stereo").toBool());
    query.bindValue(pos++, message.property("heater1").toBool());
    query.bindValue(pos++, message.property("heater2").toBool());
    query.bindValue(pos++, message.property("filter").toBool());
    query.bindValue(pos++, message.property("onzen").toBool());
    query.bindValue(pos++, message.property("ozonePeak1").toBool());
    query.bindValue(pos++, message.property("ozonePeak2").toBool());
    query.bindValue(pos++, message.property("exhaustFan").toBool());
    query.bindValue(pos++, message.property("powerlines").toInt());
    query.bindValue(pos++, message.property("breakerSize").toInt());
    query.bindValue(pos++, message.property("smartOnzen").toInt());
    query.bindValue(pos++, message.property("fogger").toBool());
    query.bindValue(pos++, message.property("sds").toBool());
    query.bindValue(pos++, message.property("yess").toBool());

    query.exec();

    qCDebug(dbLog) << "wrote message" << message.staticMetaObject.className();
}
void DatabaseClient::logMessageError(const asdc::proto::Error &message, const QDateTime &messageReceivedAt) {
    QSqlQuery query(m_database);

    query.prepare(QLatin1String(R"(
        INSERT INTO "message_error" (
            "created_at",
            "process_run_id",
            "connection_session_id",
            "message_received_at",

            "no_flow",
            "flow_switch",
            "heater_over_temperature",
            "spa_over_temperature",
            "spa_temperature_probe",
            "spa_high_limit",
            "eeprom",
            "freeze_protect",
            "ph_high",
            "heater_probe_disconnected"
        )
        VALUES (DATETIME('now'), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )"));

    int pos = 0;

    query.bindValue(pos++, m_processRunId);
    query.bindValue(pos++, m_connectionSessionId);
    query.bindValue(pos++, messageReceivedAt.toString("yyyy-MM-dd hh:mm:ss"));

    query.bindValue(pos++, message.property("noFlow").toBool());
    query.bindValue(pos++, message.property("flowSwitch").toBool());
    query.bindValue(pos++, message.property("heaterOverTemperature").toBool());
    query.bindValue(pos++, message.property("spaOverTemperature").toBool());
    query.bindValue(pos++, message.property("spaTemperatureProbe").toBool());
    query.bindValue(pos++, message.property("spaHighLimit").toBool());
    query.bindValue(pos++, message.property("eeprom").toBool());
    query.bindValue(pos++, message.property("freezeProtect").toBool());
    query.bindValue(pos++, message.property("phHigh").toBool());
    query.bindValue(pos++, message.property("heaterProbeDisconnected").toBool());

    query.exec();

    qCDebug(dbLog) << "wrote message" << message.staticMetaObject.className();
}
void DatabaseClient::logMessageFilter(const asdc::proto::Filter &message, const QDateTime &messageReceivedAt) {
    QSqlQuery query(m_database);

    query.prepare(QLatin1String(R"(
        INSERT INTO "message_filter" (
            "created_at",
            "process_run_id",
            "connection_session_id",
            "message_received_at",

            "serial_nums",
            "filter_state",
            "install_dates"
        )
        VALUES (DATETIME('now'), ?, ?, ?, ?, ?, ?)
    )"));

    int pos = 0;

    query.bindValue(pos++, m_processRunId);
    query.bindValue(pos++, m_connectionSessionId);
    query.bindValue(pos++, messageReceivedAt.toString("yyyy-MM-dd hh:mm:ss"));

    query.bindValue(pos++, message.property("serialNums").toString());
    query.bindValue(pos++, message.property("filterState").toInt());
    query.bindValue(pos++, message.property("installDates").toString());

    query.exec();

    qCDebug(dbLog) << "wrote message" << message.staticMetaObject.className();
}
void DatabaseClient::logMessageInformation(const asdc::proto::Information &message, const QDateTime &messageReceivedAt) {
    QSqlQuery query(m_database);

    query.prepare(QLatin1String(R"(
        INSERT INTO "message_information" (
            "created_at",
            "process_run_id",
            "connection_session_id",
            "message_received_at",

            "pack_serial_number",
            "pack_firmware_version",
            "pack_hardware_version",
            "pack_product_id",
            "pack_board_id",
            "topside_product_id",
            "topside_software_version",
            "guid",
            "spa_type",
            "website_registration",
            "website_registration_confirm",
            "mac_address",
            "firmware_version",
            "product_code",
            "var_software_version",
            "spaboy_firmware_version",
            "spaboy_hardware_version",
            "spaboy_product_id",
            "spaboy_serial_number",
            "rfid_firmware_version",
            "rfid_hardware_version",
            "rfid_product_id",
            "rfid_serial_number"
        )
        VALUES (DATETIME('now'), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )"));

    int pos = 0;

    query.bindValue(pos++, m_processRunId);
    query.bindValue(pos++, m_connectionSessionId);
    query.bindValue(pos++, messageReceivedAt.toString("yyyy-MM-dd hh:mm:ss"));

    query.bindValue(pos++, message.property("packSerialNumber").toString());
    query.bindValue(pos++, message.property("packFirmwareVersion").toString());
    query.bindValue(pos++, message.property("packHardwareVersion").toString());
    query.bindValue(pos++, message.property("packProductId").toString());
    query.bindValue(pos++, message.property("packBoardId").toString());
    query.bindValue(pos++, message.property("topsideProductId").toString());
    query.bindValue(pos++, message.property("topsideSoftwareVersion").toString());
    query.bindValue(pos++, message.property("guid").toString());
    query.bindValue(pos++, message.property("spaType").toInt());
    query.bindValue(pos++, message.property("websiteRegistration").toBool());
    query.bindValue(pos++, message.property("websiteRegistrationConfirm").toBool());
    query.bindValue(pos++, message.property("macAddress").toByteArray());
    query.bindValue(pos++, message.property("firmwareVersion").toInt());
    query.bindValue(pos++, message.property("productCode").toInt());
    query.bindValue(pos++, message.property("varSoftwareVersion").toString());
    query.bindValue(pos++, message.property("spaboyFirmwareVersion").toString());
    query.bindValue(pos++, message.property("spaboyHardwareVersion").toString());
    query.bindValue(pos++, message.property("spaboyProductId").toString());
    query.bindValue(pos++, message.property("spaboySerialNumber").toString());
    query.bindValue(pos++, message.property("rfidFirmwareVersion").toString());
    query.bindValue(pos++, message.property("rfidHardwareVersion").toString());
    query.bindValue(pos++, message.property("rfidProductId").toString());
    query.bindValue(pos++, message.property("rfidSerialNumber").toString());

    query.exec();

    qCDebug(dbLog) << "wrote message" << message.staticMetaObject.className();
}
void DatabaseClient::logMessageLive(const asdc::proto::Live &message, const QDateTime &messageReceivedAt) {
    QSqlQuery query(m_database);

    query.prepare(QLatin1String(R"(
        INSERT INTO "message_live" (
            "created_at",
            "process_run_id",
            "connection_session_id",
            "message_received_at",

            "temperature_fahrenheit",
            "temperature_setpoint_fahrenheit",
            "pump_1",
            "pump_2",
            "pump_3",
            "pump_4",
            "pump_5",
            "blower_1",
            "blower_2",
            "lights",
            "stereo",
            "heater_1",
            "heater_2",
            "filter",
            "onzen",
            "ozone",
            "exhaust_fan",
            "sauna",
            "heater_adc",
            "sauna_time_remaining",
            "economy",
            "current_adc",
            "all_on",
            "fogger",
            "error",
            "alarm",
            "status",
            "ph",
            "orp",
            "sds",
            "yess"
        )
        VALUES (DATETIME('now'), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )"));

    int pos = 0;

    query.bindValue(pos++, m_processRunId);
    query.bindValue(pos++, m_connectionSessionId);
    query.bindValue(pos++, messageReceivedAt.toString("yyyy-MM-dd hh:mm:ss"));

    query.bindValue(pos++, message.property("temperatureFahrenheit").toInt());
    query.bindValue(pos++, message.property("temperatureSetpointFahrenheit").toInt());
    query.bindValue(pos++, message.property("pump1").toInt());
    query.bindValue(pos++, message.property("pump2").toInt());
    query.bindValue(pos++, message.property("pump3").toInt());
    query.bindValue(pos++, message.property("pump4").toInt());
    query.bindValue(pos++, message.property("pump5").toInt());
    query.bindValue(pos++, message.property("blower1").toInt());
    query.bindValue(pos++, message.property("blower2").toInt());
    query.bindValue(pos++, message.property("lights").toBool());
    query.bindValue(pos++, message.property("stereo").toBool());
    query.bindValue(pos++, message.property("heater1").toInt());
    query.bindValue(pos++, message.property("heater2").toInt());
    query.bindValue(pos++, message.property("filter").toInt());
    query.bindValue(pos++, message.property("onzen").toBool());
    query.bindValue(pos++, message.property("ozone").toInt());
    query.bindValue(pos++, message.property("exhaustFan").toBool());
    query.bindValue(pos++, message.property("sauna").toInt());
    query.bindValue(pos++, message.property("heaterAdc").toInt());
    query.bindValue(pos++, message.property("saunaTimeRemaining").toInt());
    query.bindValue(pos++, message.property("economy").toBool());
    query.bindValue(pos++, message.property("currentAdc").toInt());
    query.bindValue(pos++, message.property("allOn").toBool());
    query.bindValue(pos++, message.property("fogger").toBool());
    query.bindValue(pos++, message.property("error").toInt());
    query.bindValue(pos++, message.property("alarm").toInt());
    query.bindValue(pos++, message.property("status").toInt());
    query.bindValue(pos++, message.property("ph").toInt());
    query.bindValue(pos++, message.property("orp").toInt());
    query.bindValue(pos++, message.property("sds").toBool());
    query.bindValue(pos++, message.property("yess").toBool());

    query.exec();

    qCDebug(dbLog) << "wrote message" << message.staticMetaObject.className();
}
void DatabaseClient::logMessageOnzenLive(const asdc::proto::OnzenLive &message, const QDateTime &messageReceivedAt) {
    QSqlQuery query(m_database);

    query.prepare(QLatin1String(R"(
        INSERT INTO "message_onzen_live" (
            "created_at",
            "process_run_id",
            "connection_session_id",
            "message_received_at",

            "guid",
            "orp",
            "ph_100",
            "current",
            "voltage",
            "current_setpoint",
            "voltage_setpoint",
            "pump1",
            "pump2",
            "orp_state_machine",
            "electrode_state_machine",
            "electrode_id",
            "electrode_polarity",
            "electrode_1_resistance_1",
            "electrode_1_resistance_2",
            "electrode_2_resistance_1",
            "electrode_2_resistance_2",
            "command_mode",
            "electrode_mah",
            "ph_color",
            "orp_color",
            "electrode_wear"
        )
        VALUES (DATETIME('now'), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )"));

    int pos = 0;

    query.bindValue(pos++, m_processRunId);
    query.bindValue(pos++, m_connectionSessionId);
    query.bindValue(pos++, messageReceivedAt.toString("yyyy-MM-dd hh:mm:ss"));

    query.bindValue(pos++, message.property("guid").toString());
    query.bindValue(pos++, message.property("orp").toInt());
    query.bindValue(pos++, message.property("ph100").toInt());
    query.bindValue(pos++, message.property("current").toInt());
    query.bindValue(pos++, message.property("voltage").toInt());
    query.bindValue(pos++, message.property("currentSetpoint").toInt());
    query.bindValue(pos++, message.property("voltageSetpoint").toInt());
    query.bindValue(pos++, message.property("pump1").toBool());
    query.bindValue(pos++, message.property("pump2").toBool());
    query.bindValue(pos++, message.property("orpStateMachine").toInt());
    query.bindValue(pos++, message.property("electrodeStateMachine").toInt());
    query.bindValue(pos++, message.property("electrodeId").toInt());
    query.bindValue(pos++, message.property("electrodePolarity").toInt());
    query.bindValue(pos++, message.property("electrode1Resistance1").toInt());
    query.bindValue(pos++, message.property("electrode1Resistance2").toInt());
    query.bindValue(pos++, message.property("electrode2Resistance1").toInt());
    query.bindValue(pos++, message.property("electrode2Resistance2").toInt());
    query.bindValue(pos++, message.property("commandMode").toBool());
    query.bindValue(pos++, message.property("electrodeMAH").toInt());
    query.bindValue(pos++, message.property("phColor").toInt());
    query.bindValue(pos++, message.property("orpColor").toInt());
    query.bindValue(pos++, message.property("electrodeWear").toInt());

    query.exec();

    qCDebug(dbLog) << "wrote message" << message.staticMetaObject.className();
}
void DatabaseClient::logMessageOnzenSettings(const asdc::proto::OnzenSettings &message, const QDateTime &messageReceivedAt) {
    QSqlQuery query(m_database);

    query.prepare(QLatin1String(R"(
        INSERT INTO "message_onzen_settings" (
            "created_at",
            "process_run_id",
            "connection_session_id",
            "message_received_at",

            "guid",
            "over_voltage",
            "under_voltage",
            "over_current",
            "under_current",
            "orp_high",
            "orp_low",
            "ph_high",
            "ph_low",
            "pwm_pump1_time_on",
            "pwm_pump1_time_off",
            "sampling_interval",
            "sampling_duration",
            "pwm_pump2_time_on",
            "pwm_pump2_time_off",
            "sb_low_cl",
            "sb_caution_low_cl",
            "sb_caution_high_cl",
            "sb_high_cl",
            "sb_low_ph",
            "sb_caution_low_ph",
            "sb_caution_high_ph",
            "sb_high_ph"
        )
        VALUES (DATETIME('now'), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )"));

    int pos = 0;

    query.bindValue(pos++, m_processRunId);
    query.bindValue(pos++, m_connectionSessionId);
    query.bindValue(pos++, messageReceivedAt.toString("yyyy-MM-dd hh:mm:ss"));

    query.bindValue(pos++, message.property("guid").toString());
    query.bindValue(pos++, message.property("overVoltage").toInt());
    query.bindValue(pos++, message.property("underVoltage").toInt());
    query.bindValue(pos++, message.property("overCurrent").toInt());
    query.bindValue(pos++, message.property("underCurrent").toInt());
    query.bindValue(pos++, message.property("orpHigh").toInt());
    query.bindValue(pos++, message.property("orpLow").toInt());
    query.bindValue(pos++, message.property("phHigh").toInt());
    query.bindValue(pos++, message.property("phLow").toInt());
    query.bindValue(pos++, message.property("pwmPump1TimeOn").toInt());
    query.bindValue(pos++, message.property("pwmPump1TimeOff").toInt());
    query.bindValue(pos++, message.property("samplingInterval").toInt());
    query.bindValue(pos++, message.property("samplingDuration").toInt());
    query.bindValue(pos++, message.property("pwmPump2TimeOn").toInt());
    query.bindValue(pos++, message.property("pwmPump2TimeOff").toInt());
    query.bindValue(pos++, message.property("sbLowCl").toInt());
    query.bindValue(pos++, message.property("sbCautionLowCl").toInt());
    query.bindValue(pos++, message.property("sbCautionHighCl").toInt());
    query.bindValue(pos++, message.property("sbHighCl").toInt());
    query.bindValue(pos++, message.property("sbLowPh").toInt());
    query.bindValue(pos++, message.property("sbCautionLowPh").toInt());
    query.bindValue(pos++, message.property("sbCautionHighPh").toInt());
    query.bindValue(pos++, message.property("sbHighPh").toInt());

    query.exec();

    qCDebug(dbLog) << "wrote message" << message.staticMetaObject.className();
}
void DatabaseClient::logMessagePeak(const asdc::proto::Peak &message, const QDateTime &messageReceivedAt) {
    QSqlQuery query(m_database);

    query.prepare(QLatin1String(R"(
        INSERT INTO "message_peak" (
            "created_at",
            "process_run_id",
            "connection_session_id",
            "message_received_at",

            "peaknum",
            "peakstart1",
            "peakend1",
            "peakstart2",
            "peakend2",
            "midpeaknum",
            "midpeakstart1",
            "midpeakend1",
            "midpeakstart2",
            "midpeakend2",
            "offpeakstart",
            "offpeakend",
            "offset",
            "peakheater",
            "peakfilter",
            "peakozone",
            "midpeakheater",
            "midpeakfilter",
            "midpeakozone",
            "sat",
            "sun",
            "mon",
            "tue",
            "wed",
            "thu",
            "fri"
        )
        VALUES (DATETIME('now'), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )"));

    int pos = 0;

    query.bindValue(pos++, m_processRunId);
    query.bindValue(pos++, m_connectionSessionId);
    query.bindValue(pos++, messageReceivedAt.toString("yyyy-MM-dd hh:mm:ss"));

    query.bindValue(pos++, message.property("peaknum").toInt());
    query.bindValue(pos++, message.property("peakstart1").toInt());
    query.bindValue(pos++, message.property("peakend1").toInt());
    query.bindValue(pos++, message.property("peakstart2").toInt());
    query.bindValue(pos++, message.property("peakend2").toInt());
    query.bindValue(pos++, message.property("midpeaknum").toInt());
    query.bindValue(pos++, message.property("midpeakstart1").toInt());
    query.bindValue(pos++, message.property("midpeakend1").toInt());
    query.bindValue(pos++, message.property("midpeakstart2").toInt());
    query.bindValue(pos++, message.property("midpeakend2").toInt());
    query.bindValue(pos++, message.property("offpeakstart").toInt());
    query.bindValue(pos++, message.property("offpeakend").toInt());
    query.bindValue(pos++, message.property("offset").toInt());
    query.bindValue(pos++, message.property("peakheater").toBool());
    query.bindValue(pos++, message.property("peakfilter").toBool());
    query.bindValue(pos++, message.property("peakozone").toBool());
    query.bindValue(pos++, message.property("midpeakheater").toBool());
    query.bindValue(pos++, message.property("midpeakfilter").toBool());
    query.bindValue(pos++, message.property("midpeakozone").toBool());
    query.bindValue(pos++, message.property("sat").toBool());
    query.bindValue(pos++, message.property("sun").toBool());
    query.bindValue(pos++, message.property("mon").toBool());
    query.bindValue(pos++, message.property("tue").toBool());
    query.bindValue(pos++, message.property("wed").toBool());
    query.bindValue(pos++, message.property("thu").toBool());
    query.bindValue(pos++, message.property("fri").toBool());

    query.exec();

    qCDebug(dbLog) << "wrote message" << message.staticMetaObject.className();
}
void DatabaseClient::logMessagePeripheral(const asdc::proto::Peripheral &message, const QDateTime &messageReceivedAt) {
    QSqlQuery query(m_database);

    query.prepare(QLatin1String(R"(
        INSERT INTO "message_peripheral" (
            "created_at",
            "process_run_id",
            "connection_session_id",
            "message_received_at",

            "guid",
            "hardware_version",
            "firmware_version",
            "product_code",
            "connected"
        )
        VALUES (DATETIME('now'), ?, ?, ?, ?, ?, ?, ?, ?)
    )"));

    int pos = 0;

    query.bindValue(pos++, m_processRunId);
    query.bindValue(pos++, m_connectionSessionId);
    query.bindValue(pos++, messageReceivedAt.toString("yyyy-MM-dd hh:mm:ss"));

    query.bindValue(pos++, message.property("guid").toString());
    query.bindValue(pos++, message.property("hardwareVersion").toInt());
    query.bindValue(pos++, message.property("firmwareVersion").toInt());
    query.bindValue(pos++, message.property("productCode").toInt());
    query.bindValue(pos++, message.property("connected").toBool());

    query.exec();

    qCDebug(dbLog) << "wrote message" << message.staticMetaObject.className();
}
void DatabaseClient::logMessageSettings(const asdc::proto::Settings &message, const QDateTime &messageReceivedAt) {
    QSqlQuery query(m_database);

    query.prepare(QLatin1String(R"(
        INSERT INTO "message_settings" (
            "created_at",
            "process_run_id",
            "connection_session_id",
            "message_received_at",

            "max_filtration_frequency",
            "min_filtration_frequency",
            "filtration_frequency",
            "max_filtration_duration",
            "min_filtration_duration",
            "filtration_duration",
            "max_onzen_hours",
            "min_onzen_hours",
            "onzen_hours",
            "max_onzen_cycles",
            "min_onzen_cycles",
            "onzen_cycles",
            "max_ozone_hours",
            "min_ozone_hours",
            "ozone_hours",
            "max_ozone_cycles",
            "min_ozone_cycles",
            "ozone_cycles",
            "filter_suspension",
            "flash_lights_on_error",
            "temperature_offset",
            "sauna_duration",
            "min_temperature",
            "max_temperature",
            "filtration_offset",
            "spaboy_hours"
        )
        VALUES (DATETIME('now'), ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
    )"));

    int pos = 0;

    query.bindValue(pos++, m_processRunId);
    query.bindValue(pos++, m_connectionSessionId);
    query.bindValue(pos++, messageReceivedAt.toString("yyyy-MM-dd hh:mm:ss"));

    query.bindValue(pos++, message.property("maxFiltrationFrequency").toInt());
    query.bindValue(pos++, message.property("minFiltrationFrequency").toInt());
    query.bindValue(pos++, message.property("filtrationFrequency").toInt());
    query.bindValue(pos++, message.property("maxFiltrationDuration").toInt());
    query.bindValue(pos++, message.property("minFiltrationDuration").toInt());
    query.bindValue(pos++, message.property("filtrationDuration").toInt());
    query.bindValue(pos++, message.property("maxOnzenHours").toInt());
    query.bindValue(pos++, message.property("minOnzenHours").toInt());
    query.bindValue(pos++, message.property("onzenHours").toInt());
    query.bindValue(pos++, message.property("maxOnzenCycles").toInt());
    query.bindValue(pos++, message.property("minOnzenCycles").toInt());
    query.bindValue(pos++, message.property("onzenCycles").toInt());
    query.bindValue(pos++, message.property("maxOzoneHours").toInt());
    query.bindValue(pos++, message.property("minOzoneHours").toInt());
    query.bindValue(pos++, message.property("ozoneHours").toInt());
    query.bindValue(pos++, message.property("maxOzoneCycles").toInt());
    query.bindValue(pos++, message.property("minOzoneCycles").toInt());
    query.bindValue(pos++, message.property("ozoneCycles").toInt());
    query.bindValue(pos++, message.property("filterSuspension").toBool());
    query.bindValue(pos++, message.property("flashLightsOnError").toBool());
    query.bindValue(pos++, message.property("temperatureOffset").toInt());
    query.bindValue(pos++, message.property("saunaDuration").toInt());
    query.bindValue(pos++, message.property("minTemperature").toInt());
    query.bindValue(pos++, message.property("maxTemperature").toInt());
    query.bindValue(pos++, message.property("filtrationOffset").toInt());
    query.bindValue(pos++, message.property("spaboyHours").toInt());

    query.exec();

    qCDebug(dbLog) << "wrote message" << message.staticMetaObject.className();
}

void DatabaseClient::logCommand(const QString &name, const QVariant &valueFrom, const QVariant &valueTo, const QDateTime &commandSentAt) {
    QSqlQuery query(m_database);

    query.prepare(QLatin1String(R"(
        INSERT INTO "command_request" (
            "created_at",
            "process_run_id",
            "connection_session_id",
            "command_sent_at",

            "name",
            "value_from",
            "value_to"
        )
        VALUES (DATETIME('now'), ?, ?, ?, ?, ?, ?)
    )"));

    int pos = 0;

    query.bindValue(pos++, m_processRunId);
    query.bindValue(pos++, m_connectionSessionId);
    query.bindValue(pos++, commandSentAt.toString("yyyy-MM-dd hh:mm:ss"));

    query.bindValue(pos++, name);
    query.bindValue(pos++, valueFrom);
    query.bindValue(pos++, valueTo);

    query.exec();

    qCDebug(dbLog) << "logged command:" << name << "- value:" << valueTo << "- sent at:" << commandSentAt;
}

};  // namespace asdc::db

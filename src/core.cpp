#include "core.h"

#include <QThread>
#include <QTimer>


#include "net/discovery.h"
#include "net/client.h"
#include "db/client.h"


// #include <QDir>
// QDir dir(":/");
// for (auto entry : dir.entryList()) {
//     qWarning() << "Found entry in resources:" << entry;
// }


namespace asdc::core {


CoreInterface::CoreInterface(QObject *parent)
    : QObject{parent}
{
    m_databaseClient = new asdc::db::DatabaseClient(this);
    m_discoveryClientWorkerThread = new QThread(this);
    m_networkClientWorkerThread = new QThread(this);
    m_autoRefreshTimer = new QTimer(this);

    initDatabase();
    initDiscovery();
    initNetwork();

    m_discoveryClientWorkerThread->start();
    m_networkClientWorkerThread->start();

    connect(m_autoRefreshTimer, &QTimer::timeout, this, &CoreInterface::autoRefreshCheck);
}
CoreInterface::~CoreInterface() {
    qDebug() << "quitting worker threads";

    m_networkClientWorkerThread->quit();
    m_networkClientWorkerThread->wait();

    m_discoveryClientWorkerThread->quit();
    m_discoveryClientWorkerThread->wait();
}

void CoreInterface::initDatabase() {
    m_databaseClient->openDatabase(true);
}
void CoreInterface::initDiscovery()
{
    asdc::net::DiscoveryClientWorker *worker = new asdc::net::DiscoveryClientWorker();
    worker->moveToThread(m_discoveryClientWorkerThread);

    connect(m_discoveryClientWorkerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(m_discoveryClientWorkerThread, &QThread::finished, m_discoveryClientWorkerThread, &QObject::deleteLater);

    connect(worker, &asdc::net::DiscoveryClientWorker::startedSearch, this, &CoreInterface::onDiscoveryClientWorkerStartedSearch);
    connect(worker, &asdc::net::DiscoveryClientWorker::finishedSearch, this, &CoreInterface::onDiscoveryClientWorkerFinishedSearch);
    connect(worker, &asdc::net::DiscoveryClientWorker::hostFound, this, &CoreInterface::onDiscoveryClientWorkerHostFound);
    connect(worker, &asdc::net::DiscoveryClientWorker::hostFound, this, &CoreInterface::discoveryHostFound);

    connect(this, &CoreInterface::discoveryClientWorkerSearch, worker, &asdc::net::DiscoveryClientWorker::search);
}
void CoreInterface::initNetwork()
{
    asdc::net::NetworkClientWorker *worker = new asdc::net::NetworkClientWorker();
    worker->moveToThread(m_networkClientWorkerThread);

    connect(m_networkClientWorkerThread, &QThread::finished, worker, &asdc::net::NetworkClientWorker::disconnectFromDevice);
    connect(m_networkClientWorkerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(m_networkClientWorkerThread, &QThread::finished, m_networkClientWorkerThread, &QObject::deleteLater);

    connect(worker, &asdc::net::NetworkClientWorker::hostChanged, this, &CoreInterface::onNetworkClientWorkerHostChanged);
    connect(worker, &asdc::net::NetworkClientWorker::connected, this, &CoreInterface::onNetworkClientWorkerConnected);
    connect(worker, &asdc::net::NetworkClientWorker::disconnected, this, &CoreInterface::onNetworkClientWorkerDisconnected);
    connect(worker, &asdc::net::NetworkClientWorker::stateChanged, this, &CoreInterface::onNetworkClientWorkerStateChanged);
    connect(worker, &asdc::net::NetworkClientWorker::errorOccurred, this, &CoreInterface::onNetworkClientWorkerErrorOccurred);

    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageClock, this, &CoreInterface::setMessageClock);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageConfiguration, this, &CoreInterface::setMessageConfiguration);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageError, this, &CoreInterface::setMessageError);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageFilter, this, &CoreInterface::setMessageFilter);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageInformation, this, &CoreInterface::setMessageInformation);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageLive, this, &CoreInterface::setMessageLive);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageOnzenLive, this, &CoreInterface::setMessageOnzenLive);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageOnzenSettings, this, &CoreInterface::setMessageOnzenSettings);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessagePeak, this, &CoreInterface::setMessagePeak);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessagePeripheral, this, &CoreInterface::setMessagePeripheral);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageSettings, this, &CoreInterface::setMessageSettings);

    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageClock, m_databaseClient, &asdc::db::DatabaseClient::logMessageClock);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageConfiguration, m_databaseClient, &asdc::db::DatabaseClient::logMessageConfiguration);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageError, m_databaseClient, &asdc::db::DatabaseClient::logMessageError);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageFilter, m_databaseClient, &asdc::db::DatabaseClient::logMessageFilter);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageInformation, m_databaseClient, &asdc::db::DatabaseClient::logMessageInformation);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageLive, m_databaseClient, &asdc::db::DatabaseClient::logMessageLive);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageOnzenLive, m_databaseClient, &asdc::db::DatabaseClient::logMessageOnzenLive);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageOnzenSettings, m_databaseClient, &asdc::db::DatabaseClient::logMessageOnzenSettings);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessagePeak, m_databaseClient, &asdc::db::DatabaseClient::logMessagePeak);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessagePeripheral, m_databaseClient, &asdc::db::DatabaseClient::logMessagePeripheral);
    connect(worker, &asdc::net::NetworkClientWorker::receivedMessageSettings, m_databaseClient, &asdc::db::DatabaseClient::logMessageSettings);

    connect(this, &CoreInterface::networkClientWorkerConnectToDevice, worker, &asdc::net::NetworkClientWorker::connectToDevice);
    connect(this, &CoreInterface::networkClientWorkerDisconnectFromDevice, worker, &asdc::net::NetworkClientWorker::disconnectFromDevice);
    connect(this, &CoreInterface::networkClientWorkerQueueMessage, worker, &asdc::net::NetworkClientWorker::queueMessage);
    connect(this, &CoreInterface::networkClientWorkerQueueCommand, worker, &asdc::net::NetworkClientWorker::queueCommand);
}

void CoreInterface::autoRefreshCheck() {
    if (m_networkState != QAbstractSocket::SocketState::ConnectedState) {
        m_autoRefreshTimer->stop();
        return;
    }

    const int maxMessageAgeSecs = 60;
    const QDateTime maxMessageAgeDateTime = QDateTime::currentDateTime().addSecs(-maxMessageAgeSecs);

    if (m_messageLiveReceivedAt.isValid() && m_messageLiveReceivedAt < maxMessageAgeDateTime) {
        qDebug() << "refreshing stale message: type = LIVE, received =" << m_messageLiveReceivedAt << ", threshold =" << maxMessageAgeDateTime;
        refreshMessageLive();
    }
    if (m_messageOnzenLiveReceivedAt.isValid() && m_messageOnzenLiveReceivedAt < maxMessageAgeDateTime) {
        qDebug() << "refreshing stale message: type = ONZEN_LIVE, received =" << m_messageOnzenLiveReceivedAt << ", threshold =" << maxMessageAgeDateTime;
        refreshMessageOnzenLive();
    }
    if (m_messageErrorReceivedAt.isValid() && m_messageErrorReceivedAt < maxMessageAgeDateTime) {
        qDebug() << "refreshing stale message: type = ERROR, received =" << m_messageErrorReceivedAt << ", threshold =" << maxMessageAgeDateTime;
        refreshMessageError();
    }
}


void CoreInterface::testMode()
{
    m_databaseClient->createConnectionSession("192.168.0.1");

    m_messageSettings.setMinTemperature(52);
    m_messageSettings.setMaxTemperature(104);
    setMessageSettings(m_messageSettings, QDateTime::currentDateTime());
    m_databaseClient->logMessageSettings(m_messageSettings, QDateTime::currentDateTime());

    // --------------------------------------------------
    // Live
    int count = 60;
    for (int i = count; i >= 0; i--) {
        int seconds = i * 60;
        QDateTime receivedAt = QDateTime::currentDateTime().addSecs(-seconds);

        m_messageLive.setTemperatureFahrenheit(i * 2);

        m_databaseClient->logMessageLive(m_messageLive, receivedAt);
    }

    m_messageLiveReceivedAt = QDateTime::currentDateTime();
    m_messageLive.setTemperatureFahrenheit(101);
    m_messageLive.setTemperatureSetpointFahrenheit(101);
    m_messageLive.setPump1(asdc::proto::Live::PumpStatus(1));
    setMessageLive(m_messageLive, QDateTime::currentDateTime());
    m_databaseClient->logMessageLive(m_messageLive, m_messageLiveReceivedAt);

    // --------------------------------------------------
    // OnzenLive
    m_messageOnzenLive.setGuid("A0-test");
    m_messageOnzenLive.setOrp(100);
    m_messageOnzenLive.setPh100(600);
    m_messageOnzenLive.setOrpColor(asdc::proto::OnzenLive::Color::COLOR_LOW);
    m_messageOnzenLive.setPhColor(asdc::proto::OnzenLive::Color::COLOR_LOW);
    setMessageOnzenLive(m_messageOnzenLive, QDateTime::currentDateTime());
    m_databaseClient->logMessageOnzenLive(m_messageOnzenLive, QDateTime::currentDateTime());

    m_messageOnzenLive.setGuid("B0-test");
    m_messageOnzenLive.setOrp(500);
    m_messageOnzenLive.setPh100(690);
    m_messageOnzenLive.setOrpColor(asdc::proto::OnzenLive::Color::COLOR_CAUTION_LOW);
    m_messageOnzenLive.setPhColor(asdc::proto::OnzenLive::Color::COLOR_CAUTION_LOW);
    setMessageOnzenLive(m_messageOnzenLive, QDateTime::currentDateTime());
    m_databaseClient->logMessageOnzenLive(m_messageOnzenLive, QDateTime::currentDateTime());

    m_messageOnzenLive.setGuid("B1-test");
    m_messageOnzenLive.setOrp(620);
    m_messageOnzenLive.setPh100(720);
    m_messageOnzenLive.setOrpColor(asdc::proto::OnzenLive::Color::COLOR_OK);
    m_messageOnzenLive.setPhColor(asdc::proto::OnzenLive::Color::COLOR_OK);
    setMessageOnzenLive(m_messageOnzenLive, QDateTime::currentDateTime());
    m_databaseClient->logMessageOnzenLive(m_messageOnzenLive, QDateTime::currentDateTime());

    m_messageOnzenLive.setGuid("C0-test");
    m_messageOnzenLive.setOrp(710);
    m_messageOnzenLive.setPh100(750);
    m_messageOnzenLive.setOrpColor(asdc::proto::OnzenLive::Color::COLOR_CAUTION_HIGH);
    m_messageOnzenLive.setPhColor(asdc::proto::OnzenLive::Color::COLOR_CAUTION_HIGH);
    setMessageOnzenLive(m_messageOnzenLive, QDateTime::currentDateTime());
    m_databaseClient->logMessageOnzenLive(m_messageOnzenLive, QDateTime::currentDateTime());

    m_messageOnzenLive.setGuid("D0-test");
    m_messageOnzenLive.setOrp(850);
    m_messageOnzenLive.setPh100(794);
    m_messageOnzenLive.setOrpColor(asdc::proto::OnzenLive::Color::COLOR_HIGH);
    m_messageOnzenLive.setPhColor(asdc::proto::OnzenLive::Color::COLOR_HIGH);
    setMessageOnzenLive(m_messageOnzenLive, QDateTime::currentDateTime());
    m_databaseClient->logMessageOnzenLive(m_messageOnzenLive, QDateTime::currentDateTime());

    m_messageOnzenLive.setGuid("test");
    m_messageOnzenLive.setOrp(615);
    m_messageOnzenLive.setPh100(722);
    m_messageOnzenLive.setOrpColor(asdc::proto::OnzenLive::Color::COLOR_OK);
    m_messageOnzenLive.setPhColor(asdc::proto::OnzenLive::Color::COLOR_OK);
    setMessageOnzenLive(m_messageOnzenLive, QDateTime::currentDateTime());
    m_databaseClient->logMessageOnzenLive(m_messageOnzenLive, QDateTime::currentDateTime());
}


void CoreInterface::discoverySearch()
{
    if (!m_discoveryWorking) {
        emit discoveryClientWorkerSearch();
    }
}

void CoreInterface::networkConnectToDevice(const QString &host) {
    emit networkClientWorkerConnectToDevice(host);
}
void CoreInterface::networkDisconnectFromDevice() {
    emit networkClientWorkerDisconnectFromDevice();
}

void CoreInterface::onDiscoveryClientWorkerStartedSearch() {
    setDiscoveryWorking(true);
    emit discoveryStartedSearch();
}
void CoreInterface::onDiscoveryClientWorkerFinishedSearch() {
    setDiscoveryWorking(false);
    emit discoveryFinishedSearch();
}
void CoreInterface::onDiscoveryClientWorkerHostFound(const QString &host) {
    qDebug() << "found host" << host;
}

void CoreInterface::onNetworkClientWorkerHostChanged(const QString &host) {
    m_networkHost = host;
    m_databaseClient->createConnectionSession(m_networkHost);
    qDebug() << m_networkHost;
    emit networkHostChanged();
}
void CoreInterface::onNetworkClientWorkerConnected() {
    m_autoRefreshTimer->start(5000);
    qDebug() << "worker client connected";
    emit networkConnected();
}
void CoreInterface::onNetworkClientWorkerDisconnected() {
    m_autoRefreshTimer->stop();
    qDebug() << "worker client disconnected";
    emit networkDisconnected();
}
void CoreInterface::onNetworkClientWorkerStateChanged(QAbstractSocket::SocketState socketState) {
    m_networkState = socketState;
    qDebug() << m_networkState;
    emit networkStateChanged();
}
void CoreInterface::onNetworkClientWorkerErrorOccurred(QAbstractSocket::SocketError socketError) {
    m_networkError = socketError;
    qDebug() << m_networkError;
    emit networkErrorOccurred();
}

bool CoreInterface::discoveryWorking() const {
    return m_discoveryWorking;
}

QString CoreInterface::networkHost() const {
    return m_networkHost;
}
QAbstractSocket::SocketState CoreInterface::networkState() const {
    return m_networkState;
}
QAbstractSocket::SocketError CoreInterface::networkError() const {
    return m_networkError;
}

asdc::proto::Clock CoreInterface::messageClock() const {
    return m_messageClock;
}
asdc::proto::Configuration CoreInterface::messageConfiguration() const {
    return m_messageConfiguration;
}
asdc::proto::Error CoreInterface::messageError() const {
    return m_messageError;
}
asdc::proto::Filter CoreInterface::messageFilter() const {
    return m_messageFilter;
}
asdc::proto::Information CoreInterface::messageInformation() const {
    return m_messageInformation;
}
asdc::proto::Live CoreInterface::messageLive() const {
    return m_messageLive;
}
asdc::proto::OnzenLive CoreInterface::messageOnzenLive() const {
    return m_messageOnzenLive;
}
asdc::proto::OnzenSettings CoreInterface::messageOnzenSettings() const {
    return m_messageOnzenSettings;
}
asdc::proto::Peak CoreInterface::messagePeak() const {
    return m_messagePeak;
}
asdc::proto::Peripheral CoreInterface::messagePeripheral() const {
    return m_messagePeripheral;
}
asdc::proto::Settings CoreInterface::messageSettings() const {
    return m_messageSettings;
}

QDateTime CoreInterface::messageClockReceivedAt() const {
    return m_messageClockReceivedAt;
}
QDateTime CoreInterface::messageConfigurationReceivedAt() const {
    return m_messageConfigurationReceivedAt;
}
QDateTime CoreInterface::messageErrorReceivedAt() const {
    return m_messageErrorReceivedAt;
}
QDateTime CoreInterface::messageFilterReceivedAt() const {
    return m_messageFilterReceivedAt;
}
QDateTime CoreInterface::messageInformationReceivedAt() const {
    return m_messageInformationReceivedAt;
}
QDateTime CoreInterface::messageLiveReceivedAt() const {
    return m_messageLiveReceivedAt;
}
QDateTime CoreInterface::messageOnzenLiveReceivedAt() const {
    return m_messageOnzenLiveReceivedAt;
}
QDateTime CoreInterface::messageOnzenSettingsReceivedAt() const {
    return m_messageOnzenSettingsReceivedAt;
}
QDateTime CoreInterface::messagePeakReceivedAt() const {
    return m_messagePeakReceivedAt;
}
QDateTime CoreInterface::messagePeripheralReceivedAt() const {
    return m_messagePeripheralReceivedAt;
}
QDateTime CoreInterface::messageSettingsReceivedAt() const {
    return m_messageSettingsReceivedAt;
}

void CoreInterface::setDiscoveryWorking(bool working) {
    if (m_discoveryWorking == working) {
        return;
    }
    m_discoveryWorking = working;
    qDebug() << m_discoveryWorking;
    emit discoveryWorkingChanged();
}

void CoreInterface::setMessageClock(const asdc::proto::Clock &message, const QDateTime &messageReceivedAt) {
    m_messageClock = message;
    m_messageClockReceivedAt = messageReceivedAt;
    emit messageClockChanged();
}
void CoreInterface::setMessageConfiguration(const asdc::proto::Configuration &message, const QDateTime &messageReceivedAt) {
    m_messageConfiguration = message;
    m_messageConfigurationReceivedAt = messageReceivedAt;
    emit messageConfigurationChanged();
}
void CoreInterface::setMessageError(const asdc::proto::Error &message, const QDateTime &messageReceivedAt) {
    m_messageError = message;
    m_messageErrorReceivedAt = messageReceivedAt;
    emit messageErrorChanged();
}
void CoreInterface::setMessageFilter(const asdc::proto::Filter &message, const QDateTime &messageReceivedAt) {
    m_messageFilter = message;
    m_messageFilterReceivedAt = messageReceivedAt;
    emit messageFilterChanged();
}
void CoreInterface::setMessageInformation(const asdc::proto::Information &message, const QDateTime &messageReceivedAt) {
    m_messageInformation = message;
    m_messageInformationReceivedAt = messageReceivedAt;
    emit messageInformationChanged();
}
void CoreInterface::setMessageLive(const asdc::proto::Live &message, const QDateTime &messageReceivedAt) {
    m_messageLive = message;
    m_messageLiveReceivedAt = messageReceivedAt;
    emit messageLiveChanged();
}
void CoreInterface::setMessageOnzenLive(const asdc::proto::OnzenLive &message, const QDateTime &messageReceivedAt) {
    m_messageOnzenLive = message;
    m_messageOnzenLiveReceivedAt = messageReceivedAt;
    emit messageOnzenLiveChanged();
}
void CoreInterface::setMessageOnzenSettings(const asdc::proto::OnzenSettings &message, const QDateTime &messageReceivedAt) {
    m_messageOnzenSettings = message;
    m_messageOnzenSettingsReceivedAt = messageReceivedAt;
    emit messageOnzenSettingsChanged();
}
void CoreInterface::setMessagePeak(const asdc::proto::Peak &message, const QDateTime &messageReceivedAt) {
    m_messagePeak = message;
    m_messagePeakReceivedAt = messageReceivedAt;
    emit messagePeakChanged();
}
void CoreInterface::setMessagePeripheral(const asdc::proto::Peripheral &message, const QDateTime &messageReceivedAt) {
    m_messagePeripheral = message;
    m_messagePeripheralReceivedAt = messageReceivedAt;
    emit messagePeripheralChanged();
}
void CoreInterface::setMessageSettings(const asdc::proto::Settings &message, const QDateTime &messageReceivedAt) {
    m_messageSettings = message;
    m_messageSettingsReceivedAt = messageReceivedAt;
    emit messageSettingsChanged();
}

void CoreInterface::refreshMessageClock() {
    emit networkClientWorkerQueueMessage(asdc::net::MessageType::CLOCK);
}
void CoreInterface::refreshMessageConfiguration() {
    emit networkClientWorkerQueueMessage(asdc::net::MessageType::CONFIGURATION);
}
void CoreInterface::refreshMessageError() {
    emit networkClientWorkerQueueMessage(asdc::net::MessageType::ERROR);
}
void CoreInterface::refreshMessageFilter() {
    emit networkClientWorkerQueueMessage(asdc::net::MessageType::FILTERS);
}
void CoreInterface::refreshMessageInformation() {
    emit networkClientWorkerQueueMessage(asdc::net::MessageType::INFORMATION);
}
void CoreInterface::refreshMessageLive() {
    emit networkClientWorkerQueueMessage(asdc::net::MessageType::LIVE);
}
void CoreInterface::refreshMessageOnzenLive() {
    emit networkClientWorkerQueueMessage(asdc::net::MessageType::ONZEN_LIVE);
}
void CoreInterface::refreshMessageOnzenSettings() {
    emit networkClientWorkerQueueMessage(asdc::net::MessageType::ONZEN_SETTINGS);
}
void CoreInterface::refreshMessagePeak() {
    emit networkClientWorkerQueueMessage(asdc::net::MessageType::PEAK);
}
void CoreInterface::refreshMessagePeripheral() {
    emit networkClientWorkerQueueMessage(asdc::net::MessageType::PERIPHERAL);
}
void CoreInterface::refreshMessageSettings() {
    emit networkClientWorkerQueueMessage(asdc::net::MessageType::SETTINGS);
}

bool CoreInterface::isCommandIntValid(const QString &name, qint32 value) {
    if (name == "setTemperatureSetpointFahrenheit") {
        if (value < m_messageSettings.minTemperature() || value > m_messageSettings.maxTemperature()) {
            qWarning() << "invalid value for:" << name << "- got" << value
                       << "; expected between minTemperature" << m_messageSettings.minTemperature()
                       << "and maxTemperature" << m_messageSettings.maxTemperature();
            return false;
        }
    } else if (name.startsWith("setPump") || name.startsWith("setBlower")) {
        int max = 2;
        if (value < 0 || value > max) {
            qWarning() << "invalid value for:" << name << "- got" << value
                       << "; expected between 0 and" << max;
            return false;
        }
    } else if (name == "setSaunaState") {
        int max = 4;
        if (value < 0 || value > max) {
            qWarning() << "invalid value for:" << name << "- got" << value
                       << "; expected between 0 and" << max;
            return false;
        }
    }
    return true;
}
void CoreInterface::sendCommand(const QString &name, const QVariant &value) {
    QString propertyName = name;
    propertyName.remove(0, 3);
    propertyName[0] = propertyName[0].toLower();

    QVariant currentValue = m_messageLive.property(propertyName);

    if (!currentValue.isValid()) {
        currentValue = "";
    }

    if (value == currentValue) {
        qDebug() << "requested value == current value:" << value << "==" << currentValue;
        return;
    }

    m_databaseClient->logCommand(name, currentValue, value, QDateTime::currentDateTime());
    emit networkClientWorkerQueueCommand(name, value);
}

void CoreInterface::commandSetTemperatureSetpointFahrenheit(qint32 value) {
    const QString name = "setTemperatureSetpointFahrenheit";
    if (isCommandIntValid(name, value)) {
        sendCommand(name, value);
    }
}
void CoreInterface::commandSetPump1(qint32 value) {
    const QString name = "setPump1";
    if (isCommandIntValid(name, value)) {
        sendCommand(name, value);
    }
}
void CoreInterface::commandSetPump2(qint32 value) {
    const QString name = "setPump2";
    if (isCommandIntValid(name, value)) {
        sendCommand(name, value);
    }
}
void CoreInterface::commandSetPump3(qint32 value) {
    const QString name = "setPump3";
    if (isCommandIntValid(name, value)) {
        sendCommand(name, value);
    }
}
void CoreInterface::commandSetPump4(qint32 value) {
    const QString name = "setPump4";
    if (isCommandIntValid(name, value)) {
        sendCommand(name, value);
    }
}
void CoreInterface::commandSetPump5(qint32 value) {
    const QString name = "setPump5";
    if (isCommandIntValid(name, value)) {
        sendCommand(name, value);
    }
}
void CoreInterface::commandSetBlower1(qint32 value) {
    const QString name = "setBlower1";
    if (isCommandIntValid(name, value)) {
        sendCommand(name, value);
    }
}
void CoreInterface::commandSetBlower2(qint32 value) {
    const QString name = "setBlower2";
    if (isCommandIntValid(name, value)) {
        sendCommand(name, value);
    }
}
void CoreInterface::commandSetLights(bool value) {
    const QString name = "setLights";
    sendCommand(name, value);
}
void CoreInterface::commandSetStereo(bool value) {
    const QString name = "setStereo";
    sendCommand(name, value);
}
void CoreInterface::commandSetFilter(bool value) {
    const QString name = "setFilter";
    sendCommand(name, value);
}
void CoreInterface::commandSetOnzen(bool value) {
    const QString name = "setOnzen";
    sendCommand(name, value);
}
void CoreInterface::commandSetOzone(bool value) {
    const QString name = "setOzone";
    sendCommand(name, value);
}
void CoreInterface::commandSetExhaustFan(bool value) {
    const QString name = "setExhaustFan";
    sendCommand(name, value);
}
void CoreInterface::commandSetSaunaState(qint32 value) {
    const QString name = "setSaunaState";
    if (isCommandIntValid(name, value)) {
        sendCommand(name, value);
    }
}
void CoreInterface::commandSetSaunaTimeLeft(qint32 value) {
    const QString name = "setSaunaTimeLeft";
    sendCommand(name, value);
}
void CoreInterface::commandSetAllOn(bool value) {
    const QString name = "setAllOn";
    sendCommand(name, value);
}
void CoreInterface::commandSetFogger(bool value) {
    const QString name = "setFogger";
    sendCommand(name, value);
}
void CoreInterface::commandSetSpaboyBoost(bool value) {
    const QString name = "setSpaboyBoost";
    sendCommand(name, value);
}
void CoreInterface::commandSetPackReset(bool value) {
    const QString name = "setPackReset";
    sendCommand(name, value);
}
void CoreInterface::commandSetLogDump(bool value) {
    const QString name = "setLogDump";
    sendCommand(name, value);
}
void CoreInterface::commandSetSds(bool value) {
    const QString name = "setSds";
    sendCommand(name, value);
}
void CoreInterface::commandSetYess(bool value) {
    const QString name = "setYess";
    sendCommand(name, value);
}

// void CoreInterface::requestMessage(const QString &name) {

// }
// void CoreInterface::requestCommand(const QString &name, QVariant &value) {

// }

};  // namespace asdc::core

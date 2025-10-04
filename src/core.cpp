#include "core.h"

#include <QThread>

#include "net/client.h"
#include "db/client.h"


#include "net/discovery.h"



namespace asdc::core {


NetworkClientWorker::NetworkClientWorker(QObject *parent)
    : QObject{parent}
{
    m_networkClient = new asdc::net::NetworkClient(this);

    connect(m_networkClient, &asdc::net::NetworkClient::hostChanged, this, &NetworkClientWorker::clientHostChanged);
    connect(m_networkClient, &asdc::net::NetworkClient::portChanged, this, &NetworkClientWorker::clientPortChanged);
    connect(m_networkClient, &asdc::net::NetworkClient::connected, this, &NetworkClientWorker::clientConnected);
    connect(m_networkClient, &asdc::net::NetworkClient::disconnected, this, &NetworkClientWorker::clientDisconnected);
    connect(m_networkClient, &asdc::net::NetworkClient::stateChanged, this, &NetworkClientWorker::clientStateChanged);
    connect(m_networkClient, &asdc::net::NetworkClient::errorOccurred, this, &NetworkClientWorker::clientErrorOccurred);
    connect(m_networkClient, &asdc::net::NetworkClient::headerReceived, this, &NetworkClientWorker::onHeaderReceived);
};

void NetworkClientWorker::connectClient(const QString &host) {
    m_networkClient->connectToDevice(host);
}
void NetworkClientWorker::disconnectClient() {
    m_networkClient->disconnectFromDevice();
}
void NetworkClientWorker::queueMessage(const asdc::net::MessageType &messageType) {
    qDebug() << "requesting message" << messageType;
    m_networkClient->requestMessage(messageType);
}

bool NetworkClientWorker::checkSerializerError() {
    if (m_serializer.lastError() != QAbstractProtobufSerializer::Error::None) {
        qWarning().nospace()
        << "Unable to deserialize ("
        << qToUnderlying(m_serializer.lastError()) << ")"
        << m_serializer.lastErrorString();
        return true;
    }
    return false;
}
void NetworkClientWorker::onHeaderReceived(const asdc::net::Header &header) {
    asdc::net::MessageType messageType = static_cast<asdc::net::MessageType>(header.messageType);

    if (messageType == asdc::net::MessageType::CLOCK) {
        asdc::proto::Clock message;
        message.deserialize(&m_serializer, header.payload);
        if (!checkSerializerError()) {
            emit receivedMessageClock(message);
        }
    } else if (messageType == asdc::net::MessageType::CONFIGURATION) {
        asdc::proto::Configuration message;
        message.deserialize(&m_serializer, header.payload);
        if (!checkSerializerError()) {
            emit receivedMessageConfiguration(message);
        }
    } else if (messageType == asdc::net::MessageType::ERROR) {
        asdc::proto::Error message;
        message.deserialize(&m_serializer, header.payload);
        if (!checkSerializerError()) {
            emit receivedMessageError(message);
        }
    } else if (messageType == asdc::net::MessageType::FILTERS) {
        asdc::proto::Filter message;
        message.deserialize(&m_serializer, header.payload);
        if (!checkSerializerError()) {
            emit receivedMessageFilter(message);
        }
    } else if (messageType == asdc::net::MessageType::INFORMATION) {
        asdc::proto::Information message;
        message.deserialize(&m_serializer, header.payload);
        if (!checkSerializerError()) {
            emit receivedMessageInformation(message);
        }
    } else if (messageType == asdc::net::MessageType::LIVE) {
        asdc::proto::Live message;
        message.deserialize(&m_serializer, header.payload);
        if (!checkSerializerError()) {
            emit receivedMessageLive(message);
        }
    } else if (messageType == asdc::net::MessageType::ONZEN_LIVE) {
        asdc::proto::OnzenLive message;
        message.deserialize(&m_serializer, header.payload);
        if (!checkSerializerError()) {
            emit receivedMessageOnzenLive(message);
        }
    } else if (messageType == asdc::net::MessageType::ONZEN_SETTINGS) {
        asdc::proto::OnzenSettings message;
        message.deserialize(&m_serializer, header.payload);
        if (!checkSerializerError()) {
            emit receivedMessageOnzenSettings(message);
        }
    } else if (messageType == asdc::net::MessageType::PEAK) {
        asdc::proto::Peak message;
        message.deserialize(&m_serializer, header.payload);
        if (!checkSerializerError()) {
            emit receivedMessagePeak(message);
        }
    } else if (messageType == asdc::net::MessageType::PERIPHERAL) {
        asdc::proto::Peripheral message;
        message.deserialize(&m_serializer, header.payload);
        if (!checkSerializerError()) {
            emit receivedMessagePeripheral(message);
        }
    } else if (messageType == asdc::net::MessageType::SETTINGS) {
        asdc::proto::Settings message;
        message.deserialize(&m_serializer, header.payload);
        if (!checkSerializerError()) {
            emit receivedMessageSettings(message);
        }
    }
}


Core::Core(QObject *parent)
    : QObject{parent}
{
    m_networkClientWorkerThread = new QThread(this);

    m_databaseClient = new asdc::db::DatabaseClient(this);

    NetworkClientWorker *worker = new NetworkClientWorker();
    worker->moveToThread(m_networkClientWorkerThread);

    connect(m_networkClientWorkerThread, &QThread::finished, worker, &NetworkClientWorker::disconnectClient);
    connect(m_networkClientWorkerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(m_networkClientWorkerThread, &QThread::finished, m_networkClientWorkerThread, &QObject::deleteLater);

    connect(worker, &NetworkClientWorker::clientHostChanged, this, &Core::onWorkerClientHostChanged);
    connect(worker, &NetworkClientWorker::clientConnected, this, &Core::onWorkerClientConnected);
    connect(worker, &NetworkClientWorker::clientDisconnected, this, &Core::onWorkerClientDisconnected);
    connect(worker, &NetworkClientWorker::clientStateChanged, this, &Core::onWorkerClientStateChanged);
    connect(worker, &NetworkClientWorker::clientErrorOccurred, this, &Core::onWorkerClientErrorOccurred);

    connect(this, &Core::workerClientConnect, worker, &NetworkClientWorker::connectClient);
    connect(this, &Core::workerClientDisconnect, worker, &NetworkClientWorker::disconnectClient);
    connect(this, &Core::workerClientQueueMessage, worker, &NetworkClientWorker::queueMessage);

    connect(worker, &NetworkClientWorker::receivedMessageClock, this, &Core::setMessageClock);
    connect(worker, &NetworkClientWorker::receivedMessageConfiguration, this, &Core::setMessageConfiguration);
    connect(worker, &NetworkClientWorker::receivedMessageError, this, &Core::setMessageError);
    connect(worker, &NetworkClientWorker::receivedMessageFilter, this, &Core::setMessageFilter);
    connect(worker, &NetworkClientWorker::receivedMessageInformation, this, &Core::setMessageInformation);
    connect(worker, &NetworkClientWorker::receivedMessageLive, this, &Core::setMessageLive);
    connect(worker, &NetworkClientWorker::receivedMessageOnzenLive, this, &Core::setMessageOnzenLive);
    connect(worker, &NetworkClientWorker::receivedMessageOnzenSettings, this, &Core::setMessageOnzenSettings);
    connect(worker, &NetworkClientWorker::receivedMessagePeak, this, &Core::setMessagePeak);
    connect(worker, &NetworkClientWorker::receivedMessagePeripheral, this, &Core::setMessagePeripheral);
    connect(worker, &NetworkClientWorker::receivedMessageSettings, this, &Core::setMessageSettings);

    connect(
        this,
        &Core::messageClockChanged,
        m_databaseClient,
        [this]() { m_databaseClient->writeMessage(asdc::net::MessageType::CLOCK, &m_messageClock, m_messageClockReceivedAt); }
    );
    connect(
        this,
        &Core::messageConfigurationChanged,
        m_databaseClient,
        [this]() { m_databaseClient->writeMessage(asdc::net::MessageType::CONFIGURATION, &m_messageConfiguration, m_messageConfigurationReceivedAt); }
    );
    connect(
        this,
        &Core::messageErrorChanged,
        m_databaseClient,
        [this]() { m_databaseClient->writeMessage(asdc::net::MessageType::ERROR, &m_messageError, m_messageErrorReceivedAt); }
    );
    connect(
        this,
        &Core::messageFilterChanged,
        m_databaseClient,
        [this]() { m_databaseClient->writeMessage(asdc::net::MessageType::FILTERS, &m_messageFilter, m_messageFilterReceivedAt); }
    );
    connect(
        this,
        &Core::messageInformationChanged,
        m_databaseClient,
        [this]() { m_databaseClient->writeMessage(asdc::net::MessageType::INFORMATION, &m_messageInformation, m_messageInformationReceivedAt); }
    );
    connect(
        this,
        &Core::messageLiveChanged,
        m_databaseClient,
        [this]() { m_databaseClient->writeMessage(asdc::net::MessageType::LIVE, &m_messageLive, m_messageLiveReceivedAt); }
    );
    connect(
        this,
        &Core::messageOnzenLiveChanged,
        m_databaseClient,
        [this]() { m_databaseClient->writeMessage(asdc::net::MessageType::ONZEN_LIVE, &m_messageOnzenLive, m_messageOnzenLiveReceivedAt); }
    );
    connect(
        this,
        &Core::messageOnzenSettingsChanged,
        m_databaseClient,
        [this]() { m_databaseClient->writeMessage(asdc::net::MessageType::ONZEN_SETTINGS, &m_messageOnzenSettings, m_messageOnzenSettingsReceivedAt); }
    );
    connect(
        this,
        &Core::messagePeakChanged,
        m_databaseClient,
        [this]() { m_databaseClient->writeMessage(asdc::net::MessageType::PEAK, &m_messagePeak, m_messagePeakReceivedAt); }
    );
    connect(
        this,
        &Core::messagePeripheralChanged,
        m_databaseClient,
        [this]() { m_databaseClient->writeMessage(asdc::net::MessageType::PERIPHERAL, &m_messagePeripheral, m_messagePeripheralReceivedAt); }
    );
    connect(
        this,
        &Core::messageSettingsChanged,
        m_databaseClient,
        [this]() { m_databaseClient->writeMessage(asdc::net::MessageType::SETTINGS, &m_messageSettings, m_messageSettingsReceivedAt); }
    );

    m_databaseClient->openDatabase(true);

    m_networkClientWorkerThread->start();


    test();
}
Core::~Core() {
    qDebug() << "quitting worker thread";
    m_networkClientWorkerThread->quit();
    m_networkClientWorkerThread->wait();
    qDebug() << "quit worker thread";
}

void Core::testDiscovery() {
    asdc::net::DiscoveryClient discoveryClient;
    discoveryClient.test();

    discoveryClient.setIpAndMask("192.168.2.14", 24);
    QStringList results = discoveryClient.search(/*timeoutMs*/ 1000, /*maxWorkers*/ 50);
    qInfo() << "Found" << results.size() << "devices:" << results;
}

void Core::test() {

    m_databaseClient->createConnectionSession("192.168.0.1");

    // --------------------------------------------------
    // Live
    int count = 60;
    for (int i = count; i >= 0; i--) {
        int seconds = i * 60;
        QDateTime receivedAt = QDateTime::currentDateTime().addSecs(-seconds);

        m_messageLive.setTemperatureFahrenheit(i * 2);

        m_databaseClient->writeMessage(asdc::net::MessageType::LIVE, &m_messageLive, receivedAt);
    }

    m_messageLiveReceivedAt = QDateTime::currentDateTime();
    m_messageLive.setTemperatureFahrenheit(101);
    m_messageLive.setTemperatureSetpointFahrenheit(103);
    m_messageLive.setPump1(asdc::proto::Live::PumpStatus(1));
    setMessageLive(m_messageLive);


    // --------------------------------------------------
    // OnzenLive
    m_messageOnzenLive.setGuid("A0-test");
    m_messageOnzenLive.setOrp(100);
    m_messageOnzenLive.setPh100(600);
    m_messageOnzenLive.setOrpColor(asdc::proto::OnzenLive::Color::COLOR_LOW);
    m_messageOnzenLive.setPhColor(asdc::proto::OnzenLive::Color::COLOR_LOW);
    setMessageOnzenLive(m_messageOnzenLive);

    m_messageOnzenLive.setGuid("B0-test");
    m_messageOnzenLive.setOrp(500);
    m_messageOnzenLive.setPh100(690);
    m_messageOnzenLive.setOrpColor(asdc::proto::OnzenLive::Color::COLOR_CAUTION_LOW);
    m_messageOnzenLive.setPhColor(asdc::proto::OnzenLive::Color::COLOR_CAUTION_LOW);
    setMessageOnzenLive(m_messageOnzenLive);

    m_messageOnzenLive.setGuid("B1-test");
    m_messageOnzenLive.setOrp(620);
    m_messageOnzenLive.setPh100(720);
    m_messageOnzenLive.setOrpColor(asdc::proto::OnzenLive::Color::COLOR_OK);
    m_messageOnzenLive.setPhColor(asdc::proto::OnzenLive::Color::COLOR_OK);
    setMessageOnzenLive(m_messageOnzenLive);

    m_messageOnzenLive.setGuid("C0-test");
    m_messageOnzenLive.setOrp(710);
    m_messageOnzenLive.setPh100(750);
    m_messageOnzenLive.setOrpColor(asdc::proto::OnzenLive::Color::COLOR_CAUTION_HIGH);
    m_messageOnzenLive.setPhColor(asdc::proto::OnzenLive::Color::COLOR_CAUTION_HIGH);
    setMessageOnzenLive(m_messageOnzenLive);

    m_messageOnzenLive.setGuid("D0-test");
    m_messageOnzenLive.setOrp(850);
    m_messageOnzenLive.setPh100(794);
    m_messageOnzenLive.setOrpColor(asdc::proto::OnzenLive::Color::COLOR_HIGH);
    m_messageOnzenLive.setPhColor(asdc::proto::OnzenLive::Color::COLOR_HIGH);
    setMessageOnzenLive(m_messageOnzenLive);


    m_messageOnzenLive.setGuid("test");
    m_messageOnzenLive.setOrp(615);
    m_messageOnzenLive.setPh100(722);
    m_messageOnzenLive.setOrpColor(asdc::proto::OnzenLive::Color::COLOR_OK);
    m_messageOnzenLive.setPhColor(asdc::proto::OnzenLive::Color::COLOR_OK);
    setMessageOnzenLive(m_messageOnzenLive);


    m_messageSettings.setMinTemperature(52);
    m_messageSettings.setMaxTemperature(104);
    setMessageSettings(m_messageSettings);
}

void Core::connectClient(const QString &host) {
    emit workerClientConnect(host);
}
void Core::disconnectClient() {
    emit workerClientDisconnect();
}

QString Core::clientHost() const {
    return m_clientHost;
}
QAbstractSocket::SocketState Core::clientState() const {
    return m_clientState;
}
QAbstractSocket::SocketError Core::clientError() const {
    return m_clientError;
}

void Core::onWorkerClientHostChanged(const QString &host) {
    m_clientHost = host;
    qDebug() << m_clientHost;
    m_databaseClient->createConnectionSession(m_clientHost);
    emit clientHostChanged();
}
void Core::onWorkerClientConnected() {
    qDebug() << "worker client connected";
    emit clientConnected();
}
void Core::onWorkerClientDisconnected() {
    qDebug() << "worker client disconnected";
    emit clientDisconnected();
}
void Core::onWorkerClientStateChanged(QAbstractSocket::SocketState socketState) {
    m_clientState = socketState;
    qDebug() << m_clientState;
    emit clientStateChanged();
}
void Core::onWorkerClientErrorOccurred(QAbstractSocket::SocketError socketError) {
    m_clientError = socketError;
    qDebug() << m_clientError;
    emit clientErrorOccurred();
}

asdc::proto::Clock Core::messageClock() const {
    return m_messageClock;
}
asdc::proto::Configuration Core::messageConfiguration() const {
    return m_messageConfiguration;
}
asdc::proto::Error Core::messageError() const {
    return m_messageError;
}
asdc::proto::Filter Core::messageFilter() const {
    return m_messageFilter;
}
asdc::proto::Information Core::messageInformation() const {
    return m_messageInformation;
}
asdc::proto::Live Core::messageLive() const {
    return m_messageLive;
}
asdc::proto::OnzenLive Core::messageOnzenLive() const {
    return m_messageOnzenLive;
}
asdc::proto::OnzenSettings Core::messageOnzenSettings() const {
    return m_messageOnzenSettings;
}
asdc::proto::Peak Core::messagePeak() const {
    return m_messagePeak;
}
asdc::proto::Peripheral Core::messagePeripheral() const {
    return m_messagePeripheral;
}
asdc::proto::Settings Core::messageSettings() const {
    return m_messageSettings;
}

QDateTime Core::messageClockReceivedAt() const {
    return m_messageClockReceivedAt;
}
QDateTime Core::messageConfigurationReceivedAt() const {
    return m_messageConfigurationReceivedAt;
}
QDateTime Core::messageErrorReceivedAt() const {
    return m_messageErrorReceivedAt;
}
QDateTime Core::messageFilterReceivedAt() const {
    return m_messageFilterReceivedAt;
}
QDateTime Core::messageInformationReceivedAt() const {
    return m_messageInformationReceivedAt;
}
QDateTime Core::messageLiveReceivedAt() const {
    return m_messageLiveReceivedAt;
}
QDateTime Core::messageOnzenLiveReceivedAt() const {
    return m_messageOnzenLiveReceivedAt;
}
QDateTime Core::messageOnzenSettingsReceivedAt() const {
    return m_messageOnzenSettingsReceivedAt;
}
QDateTime Core::messagePeakReceivedAt() const {
    return m_messagePeakReceivedAt;
}
QDateTime Core::messagePeripheralReceivedAt() const {
    return m_messagePeripheralReceivedAt;
}
QDateTime Core::messageSettingsReceivedAt() const {
    return m_messageSettingsReceivedAt;
}

void Core::setMessageClock(asdc::proto::Clock message) {
    m_messageClock = message;
    m_messageClockReceivedAt = QDateTime::currentDateTime();
    emit messageClockChanged();
}
void Core::setMessageConfiguration(asdc::proto::Configuration message) {
    m_messageConfiguration = message;
    m_messageConfigurationReceivedAt = QDateTime::currentDateTime();
    emit messageConfigurationChanged();
}
void Core::setMessageError(asdc::proto::Error message) {
    m_messageError = message;
    m_messageErrorReceivedAt = QDateTime::currentDateTime();
    emit messageErrorChanged();
}
void Core::setMessageFilter(asdc::proto::Filter message) {
    m_messageFilter = message;
    m_messageFilterReceivedAt = QDateTime::currentDateTime();
    emit messageFilterChanged();
}
void Core::setMessageInformation(asdc::proto::Information message) {
    m_messageInformation = message;
    m_messageInformationReceivedAt = QDateTime::currentDateTime();
    emit messageInformationChanged();
}
void Core::setMessageLive(asdc::proto::Live message) {
    m_messageLive = message;
    m_messageLiveReceivedAt = QDateTime::currentDateTime();
    emit messageLiveChanged();
}
void Core::setMessageOnzenLive(asdc::proto::OnzenLive message) {
    m_messageOnzenLive = message;
    m_messageOnzenLiveReceivedAt = QDateTime::currentDateTime();
    emit messageOnzenLiveChanged();
}
void Core::setMessageOnzenSettings(asdc::proto::OnzenSettings message) {
    m_messageOnzenSettings = message;
    m_messageOnzenSettingsReceivedAt = QDateTime::currentDateTime();
    emit messageOnzenSettingsChanged();
}
void Core::setMessagePeak(asdc::proto::Peak message) {
    m_messagePeak = message;
    m_messagePeakReceivedAt = QDateTime::currentDateTime();
    emit messagePeakChanged();
}
void Core::setMessagePeripheral(asdc::proto::Peripheral message) {
    m_messagePeripheral = message;
    m_messagePeripheralReceivedAt = QDateTime::currentDateTime();
    emit messagePeripheralChanged();
}
void Core::setMessageSettings(asdc::proto::Settings message) {
    m_messageSettings = message;
    m_messageSettingsReceivedAt = QDateTime::currentDateTime();
    emit messageSettingsChanged();
}

void Core::refreshMessageClock() {
    emit workerClientQueueMessage(asdc::net::MessageType::CLOCK);
}
void Core::refreshMessageConfiguration() {
    emit workerClientQueueMessage(asdc::net::MessageType::CONFIGURATION);
}
void Core::refreshMessageError() {
    emit workerClientQueueMessage(asdc::net::MessageType::ERROR);
}
void Core::refreshMessageFilter() {
    emit workerClientQueueMessage(asdc::net::MessageType::FILTERS);
}
void Core::refreshMessageInformation() {
    emit workerClientQueueMessage(asdc::net::MessageType::INFORMATION);
}
void Core::refreshMessageLive() {
    emit workerClientQueueMessage(asdc::net::MessageType::LIVE);
}
void Core::refreshMessageOnzenLive() {
    emit workerClientQueueMessage(asdc::net::MessageType::ONZEN_LIVE);
}
void Core::refreshMessageOnzenSettings() {
    emit workerClientQueueMessage(asdc::net::MessageType::ONZEN_SETTINGS);
}
void Core::refreshMessagePeak() {
    emit workerClientQueueMessage(asdc::net::MessageType::PEAK);
}
void Core::refreshMessagePeripheral() {
    emit workerClientQueueMessage(asdc::net::MessageType::PERIPHERAL);
}
void Core::refreshMessageSettings() {
    emit workerClientQueueMessage(asdc::net::MessageType::SETTINGS);
}


};  // namespace asdc

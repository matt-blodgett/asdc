#include "core.h"

#include <QThread>

#include "network/client.h"


namespace asdc::core {


NetworkClientWorker::NetworkClientWorker(QObject *parent)
    : QObject{parent}
{
    m_networkClient = new asdc::network::Client(this);

    connect(m_networkClient, &asdc::network::Client::hostChanged, this, &NetworkClientWorker::clientHostChanged);
    connect(m_networkClient, &asdc::network::Client::portChanged, this, &NetworkClientWorker::clientPortChanged);
    connect(m_networkClient, &asdc::network::Client::connected, this, &NetworkClientWorker::clientConnected);
    connect(m_networkClient, &asdc::network::Client::disconnected, this, &NetworkClientWorker::clientDisconnected);
    connect(m_networkClient, &asdc::network::Client::stateChanged, this, &NetworkClientWorker::clientStateChanged);
    connect(m_networkClient, &asdc::network::Client::errorOccurred, this, &NetworkClientWorker::clientErrorOccurred);
};

void NetworkClientWorker::connectClient(const QString &host) {
    m_networkClient->connectToDevice(host);
}
void NetworkClientWorker::disconnectClient() {
    m_networkClient->disconnectFromDevice();
}
void NetworkClientWorker::queueMessage(const asdc::network::MessageType &messageType) {
    if (messageType == asdc::network::MessageType::CLOCK) {
        asdc::proto::Clock message = m_networkClient->getMessage<asdc::proto::Clock>(messageType);
        emit receivedMessageClock(message);
    }  else if (messageType == asdc::network::MessageType::CONFIGURATION) {
        asdc::proto::Configuration message = m_networkClient->getMessage<asdc::proto::Configuration>(messageType);
        emit receivedMessageConfiguration(message);
    } else if (messageType == asdc::network::MessageType::ERROR) {
        asdc::proto::Error message = m_networkClient->getMessage<asdc::proto::Error>(messageType);
        emit receivedMessageError(message);
    } else if (messageType == asdc::network::MessageType::FILTERS) {
        asdc::proto::Filter message = m_networkClient->getMessage<asdc::proto::Filter>(messageType);
        emit receivedMessageFilter(message);
    } else if (messageType == asdc::network::MessageType::INFORMATION) {
        asdc::proto::Information message = m_networkClient->getMessage<asdc::proto::Information>(messageType);
        emit receivedMessageInformation(message);
    } else if (messageType == asdc::network::MessageType::LIVE) {
        asdc::proto::Live message = m_networkClient->getMessage<asdc::proto::Live>(messageType);
        emit receivedMessageLive(message);
    } else if (messageType == asdc::network::MessageType::ONZEN_LIVE) {
        asdc::proto::OnzenLive message = m_networkClient->getMessage<asdc::proto::OnzenLive>(messageType);
        emit receivedMessageOnzenLive(message);
    } else if (messageType == asdc::network::MessageType::ONZEN_SETTINGS) {
        asdc::proto::OnzenSettings message = m_networkClient->getMessage<asdc::proto::OnzenSettings>(messageType);
        emit receivedMessageOnzenSettings(message);
    } else if (messageType == asdc::network::MessageType::PEAK) {
        asdc::proto::Peak message = m_networkClient->getMessage<asdc::proto::Peak>(messageType);
        emit receivedMessagePeak(message);
    } else if (messageType == asdc::network::MessageType::PERIPHERAL) {
        asdc::proto::Peripheral message = m_networkClient->getMessage<asdc::proto::Peripheral>(messageType);
        emit receivedMessagePeripheral(message);
    } else if (messageType == asdc::network::MessageType::SETTINGS) {
        asdc::proto::Settings message = m_networkClient->getMessage<asdc::proto::Settings>(messageType);
        emit receivedMessageSettings(message);
    }
}

Core::Core(QObject *parent)
    : QObject{parent}
{
    NetworkClientWorker *worker = new NetworkClientWorker();
    m_workerThread = new QThread(this);

    worker->moveToThread(m_workerThread);

    connect(m_workerThread, &QThread::finished, worker, &NetworkClientWorker::disconnectClient);
    connect(m_workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(m_workerThread, &QThread::finished, m_workerThread, &QObject::deleteLater);

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

    m_workerThread->start();
}
Core::~Core() {
    qDebug() << "quitting worker thread";
    m_workerThread->quit();
    m_workerThread->wait();
    qDebug() << "quit worker thread";
}

void Core::connectClient(const QString &host) {
    emit workerClientConnect(host);
}
void Core::disconnectClient() {
    emit workerClientDisconnect();
}

QString Core::getClientHost() const {
    return m_clientHost;
}
QAbstractSocket::SocketState Core::getClientState() const {
    return m_clientState;
}
QAbstractSocket::SocketError Core::getClientError() const {
    return m_clientError;
}

void Core::onWorkerClientHostChanged(const QString &host) {
    m_clientHost = host;
    qDebug() << m_clientHost;
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

asdc::proto::Clock Core::getMessageClock() const {
    return m_messageClock;
}
asdc::proto::Configuration Core::getMessageConfiguration() const {
    return m_messageConfiguration;
}
asdc::proto::Error Core::getMessageError() const {
    return m_messageError;
}
asdc::proto::Filter Core::getMessageFilter() const {
    return m_messageFilter;
}
asdc::proto::Information Core::getMessageInformation() const {
    return m_messageInformation;
}
asdc::proto::Live Core::getMessageLive() const {
    return m_messageLive;
}
asdc::proto::OnzenLive Core::getMessageOnzenLive() const {
    return m_messageOnzenLive;
}
asdc::proto::OnzenSettings Core::getMessageOnzenSettings() const {
    return m_messageOnzenSettings;
}
asdc::proto::Peak Core::getMessagePeak() const {
    return m_messagePeak;
}
asdc::proto::Peripheral Core::getMessagePeripheral() const {
    return m_messagePeripheral;
}
asdc::proto::Settings Core::getMessageSettings() const {
    return m_messageSettings;
}

void Core::setMessageClock(asdc::proto::Clock message) {
    m_messageClock = message;
    emit messageClockChanged();
}
void Core::setMessageConfiguration(asdc::proto::Configuration message) {
    m_messageConfiguration = message;
    emit messageConfigurationChanged();
}
void Core::setMessageError(asdc::proto::Error message) {
    m_messageError = message;
    emit messageErrorChanged();
}
void Core::setMessageFilter(asdc::proto::Filter message) {
    m_messageFilter = message;
    emit messageFilterChanged();
}
void Core::setMessageInformation(asdc::proto::Information message) {
    m_messageInformation = message;
    emit messageInformationChanged();
}
void Core::setMessageLive(asdc::proto::Live message) {
    m_messageLive = message;
    emit messageLiveChanged();
}
void Core::setMessageOnzenLive(asdc::proto::OnzenLive message) {
    m_messageOnzenLive = message;
    emit messageOnzenLiveChanged();
}
void Core::setMessageOnzenSettings(asdc::proto::OnzenSettings message) {
    m_messageOnzenSettings = message;
    emit messageOnzenSettingsChanged();
}
void Core::setMessagePeak(asdc::proto::Peak message) {
    m_messagePeak = message;
    emit messagePeakChanged();
}
void Core::setMessagePeripheral(asdc::proto::Peripheral message) {
    m_messagePeripheral = message;
    emit messagePeripheralChanged();
}
void Core::setMessageSettings(asdc::proto::Settings message) {
    m_messageSettings = message;
    emit messageSettingsChanged();
}

void Core::refreshMessageClock() {
    emit workerClientQueueMessage(asdc::network::MessageType::CLOCK);
}
void Core::refreshMessageConfiguration() {
    emit workerClientQueueMessage(asdc::network::MessageType::CONFIGURATION);
}
void Core::refreshMessageError() {
    emit workerClientQueueMessage(asdc::network::MessageType::ERROR);
}
void Core::refreshMessageFilter() {
    emit workerClientQueueMessage(asdc::network::MessageType::FILTERS);
}
void Core::refreshMessageInformation() {
    emit workerClientQueueMessage(asdc::network::MessageType::INFORMATION);
}
void Core::refreshMessageLive() {
    emit workerClientQueueMessage(asdc::network::MessageType::LIVE);
}
void Core::refreshMessageOnzenLive() {
    emit workerClientQueueMessage(asdc::network::MessageType::ONZEN_LIVE);
}
void Core::refreshMessageOnzenSettings() {
    emit workerClientQueueMessage(asdc::network::MessageType::ONZEN_SETTINGS);
}
void Core::refreshMessagePeak() {
    emit workerClientQueueMessage(asdc::network::MessageType::PEAK);
}
void Core::refreshMessagePeripheral() {
    emit workerClientQueueMessage(asdc::network::MessageType::PERIPHERAL);
}
void Core::refreshMessageSettings() {
    emit workerClientQueueMessage(asdc::network::MessageType::SETTINGS);
}


};  // namespace asdc

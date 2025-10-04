#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QAbstractSocket>
#include <QDateTime>

#include <QtProtobuf/QProtobufSerializer>


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
struct Header;
class NetworkClient;
}

namespace asdc::db {
class DatabaseClient;
}


namespace asdc::core {


class NetworkClientWorker : public QObject {
    Q_OBJECT

public:
    explicit NetworkClientWorker(QObject *parent = nullptr);

public slots:
    void connectClient(const QString &host);
    void disconnectClient();
    void queueMessage(const asdc::net::MessageType &messageType);

private:
    bool checkSerializerError();
    void onHeaderReceived(const asdc::net::Header &header);

private:
    asdc::net::NetworkClient *m_networkClient;
    QProtobufSerializer m_serializer;

signals:
    void clientHostChanged(const QString &host);
    void clientPortChanged(const quint16 &port);
    void clientConnected();
    void clientDisconnected();
    void clientStateChanged(QAbstractSocket::SocketState socketState);
    void clientErrorOccurred(QAbstractSocket::SocketError socketError);

    void receivedMessageClock(asdc::proto::Clock message);
    void receivedMessageConfiguration(asdc::proto::Configuration message);
    void receivedMessageError(asdc::proto::Error message);
    void receivedMessageFilter(asdc::proto::Filter message);
    void receivedMessageInformation(asdc::proto::Information message);
    void receivedMessageLive(asdc::proto::Live message);
    void receivedMessageOnzenLive(asdc::proto::OnzenLive message);
    void receivedMessageOnzenSettings(asdc::proto::OnzenSettings message);
    void receivedMessagePeak(asdc::proto::Peak message);
    void receivedMessagePeripheral(asdc::proto::Peripheral message);
    void receivedMessageSettings(asdc::proto::Settings message);
};

class Core : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString clientHost READ clientHost NOTIFY clientHostChanged)
    Q_PROPERTY(QAbstractSocket::SocketState clientState READ clientState NOTIFY clientStateChanged)
    Q_PROPERTY(QAbstractSocket::SocketError clientError READ clientError NOTIFY clientErrorOccurred)

    Q_PROPERTY(asdc::proto::Clock messageClock READ messageClock NOTIFY messageClockChanged)
    Q_PROPERTY(asdc::proto::Configuration messageConfiguration READ messageConfiguration NOTIFY messageConfigurationChanged)
    Q_PROPERTY(asdc::proto::Error messageError READ messageError NOTIFY messageErrorChanged)
    Q_PROPERTY(asdc::proto::Filter messageFilter READ messageFilter NOTIFY messageFilterChanged)
    Q_PROPERTY(asdc::proto::Information messageInformation READ messageInformation NOTIFY messageInformationChanged)
    Q_PROPERTY(asdc::proto::Live messageLive READ messageLive NOTIFY messageLiveChanged)
    Q_PROPERTY(asdc::proto::OnzenLive messageOnzenLive READ messageOnzenLive NOTIFY messageOnzenLiveChanged)
    Q_PROPERTY(asdc::proto::OnzenSettings messageOnzenSettings READ messageOnzenSettings NOTIFY messageOnzenSettingsChanged)
    Q_PROPERTY(asdc::proto::Peak messagePeak READ messagePeak NOTIFY messagePeakChanged)
    Q_PROPERTY(asdc::proto::Peripheral messagePeripheral READ messagePeripheral NOTIFY messagePeripheralChanged)
    Q_PROPERTY(asdc::proto::Settings messageSettings READ messageSettings NOTIFY messageSettingsChanged)

    Q_PROPERTY(QDateTime messageClockReceivedAt READ messageClockReceivedAt NOTIFY messageClockChanged)
    Q_PROPERTY(QDateTime messageConfigurationReceivedAt READ messageConfigurationReceivedAt NOTIFY messageConfigurationChanged)
    Q_PROPERTY(QDateTime messageErrorReceivedAt READ messageErrorReceivedAt NOTIFY messageErrorChanged)
    Q_PROPERTY(QDateTime messageFilterReceivedAt READ messageFilterReceivedAt NOTIFY messageFilterChanged)
    Q_PROPERTY(QDateTime messageInformationReceivedAt READ messageInformationReceivedAt NOTIFY messageInformationChanged)
    Q_PROPERTY(QDateTime messageLiveReceivedAt READ messageLiveReceivedAt NOTIFY messageLiveChanged)
    Q_PROPERTY(QDateTime messageOnzenLiveReceivedAt READ messageOnzenLiveReceivedAt NOTIFY messageOnzenLiveChanged)
    Q_PROPERTY(QDateTime messageOnzenSettingsReceivedAt READ messageOnzenSettingsReceivedAt NOTIFY messageOnzenSettingsChanged)
    Q_PROPERTY(QDateTime messagePeakReceivedAt READ messagePeakReceivedAt NOTIFY messagePeakChanged)
    Q_PROPERTY(QDateTime messagePeripheralReceivedAt READ messagePeripheralReceivedAt NOTIFY messagePeripheralChanged)
    Q_PROPERTY(QDateTime messageSettingsReceivedAt READ messageSettingsReceivedAt NOTIFY messageSettingsChanged)

public:
    explicit Core(QObject *parent = nullptr);
    ~Core();

    Q_INVOKABLE void testDiscovery();

private:
    void test();

public:
    Q_INVOKABLE void connectClient(const QString &host);
    Q_INVOKABLE void disconnectClient();

private slots:
    void onWorkerClientHostChanged(const QString &host);
    void onWorkerClientConnected();
    void onWorkerClientDisconnected();
    void onWorkerClientStateChanged(QAbstractSocket::SocketState socketState);
    void onWorkerClientErrorOccurred(QAbstractSocket::SocketError socketError);

public:
    QString clientHost() const;
    QAbstractSocket::SocketState clientState() const;
    QAbstractSocket::SocketError clientError() const;

    asdc::proto::Clock messageClock() const;
    asdc::proto::Configuration messageConfiguration() const;
    asdc::proto::Error messageError() const;
    asdc::proto::Filter messageFilter() const;
    asdc::proto::Information messageInformation() const;
    asdc::proto::Live messageLive() const;
    asdc::proto::OnzenLive messageOnzenLive() const;
    asdc::proto::OnzenSettings messageOnzenSettings() const;
    asdc::proto::Peak messagePeak() const;
    asdc::proto::Peripheral messagePeripheral() const;
    asdc::proto::Settings messageSettings() const;

    QDateTime messageClockReceivedAt() const;
    QDateTime messageConfigurationReceivedAt() const;
    QDateTime messageErrorReceivedAt() const;
    QDateTime messageFilterReceivedAt() const;
    QDateTime messageInformationReceivedAt() const;
    QDateTime messageLiveReceivedAt() const;
    QDateTime messageOnzenLiveReceivedAt() const;
    QDateTime messageOnzenSettingsReceivedAt() const;
    QDateTime messagePeakReceivedAt() const;
    QDateTime messagePeripheralReceivedAt() const;
    QDateTime messageSettingsReceivedAt() const;

private slots:
    void setMessageClock(asdc::proto::Clock message);
    void setMessageConfiguration(asdc::proto::Configuration message);
    void setMessageError(asdc::proto::Error message);
    void setMessageFilter(asdc::proto::Filter message);
    void setMessageInformation(asdc::proto::Information message);
    void setMessageLive(asdc::proto::Live message);
    void setMessageOnzenLive(asdc::proto::OnzenLive message);
    void setMessageOnzenSettings(asdc::proto::OnzenSettings message);
    void setMessagePeak(asdc::proto::Peak message);
    void setMessagePeripheral(asdc::proto::Peripheral message);
    void setMessageSettings(asdc::proto::Settings message);

public:
    Q_INVOKABLE void refreshMessageClock();
    Q_INVOKABLE void refreshMessageConfiguration();
    Q_INVOKABLE void refreshMessageError();
    Q_INVOKABLE void refreshMessageFilter();
    Q_INVOKABLE void refreshMessageInformation();
    Q_INVOKABLE void refreshMessageLive();
    Q_INVOKABLE void refreshMessageOnzenLive();
    Q_INVOKABLE void refreshMessageOnzenSettings();
    Q_INVOKABLE void refreshMessagePeak();
    Q_INVOKABLE void refreshMessagePeripheral();
    Q_INVOKABLE void refreshMessageSettings();

private:
    QThread *m_networkClientWorkerThread;

    asdc::db::DatabaseClient *m_databaseClient;

    QString m_clientHost;
    QAbstractSocket::SocketState m_clientState = QAbstractSocket::SocketState::UnconnectedState;
    QAbstractSocket::SocketError m_clientError = QAbstractSocket::SocketError::UnknownSocketError;

    asdc::proto::Clock m_messageClock;
    asdc::proto::Configuration m_messageConfiguration;
    asdc::proto::Error m_messageError;
    asdc::proto::Filter m_messageFilter;
    asdc::proto::Information m_messageInformation;
    asdc::proto::Live m_messageLive;
    asdc::proto::OnzenLive m_messageOnzenLive;
    asdc::proto::OnzenSettings m_messageOnzenSettings;
    asdc::proto::Peak m_messagePeak;
    asdc::proto::Peripheral m_messagePeripheral;
    asdc::proto::Settings m_messageSettings;

    QDateTime m_messageClockReceivedAt;
    QDateTime m_messageConfigurationReceivedAt;
    QDateTime m_messageErrorReceivedAt;
    QDateTime m_messageFilterReceivedAt;
    QDateTime m_messageInformationReceivedAt;
    QDateTime m_messageLiveReceivedAt;
    QDateTime m_messageOnzenLiveReceivedAt;
    QDateTime m_messageOnzenSettingsReceivedAt;
    QDateTime m_messagePeakReceivedAt;
    QDateTime m_messagePeripheralReceivedAt;
    QDateTime m_messageSettingsReceivedAt;

signals:
    void workerClientConnect(const QString &host);
    void workerClientDisconnect();
    void workerClientQueueMessage(const asdc::net::MessageType &messageType);

    void clientHostChanged();
    void clientConnected();
    void clientDisconnected();
    void clientStateChanged();
    void clientErrorOccurred();

    void messageClockChanged();
    void messageConfigurationChanged();
    void messageErrorChanged();
    void messageFilterChanged();
    void messageInformationChanged();
    void messageLiveChanged();
    void messageOnzenLiveChanged();
    void messageOnzenSettingsChanged();
    void messagePeakChanged();
    void messagePeripheralChanged();
    void messageSettingsChanged();
};

};  // namespace asdc::core

#endif // CORE_H

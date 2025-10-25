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


namespace asdc::db {
class DatabaseClient;
}

namespace asdc::net {
enum class MessageType;
}


namespace asdc::core {


class CoreInterface : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool discoveryWorking READ discoveryWorking NOTIFY discoveryWorkingChanged)

    Q_PROPERTY(QString networkHost READ networkHost NOTIFY networkHostChanged)
    Q_PROPERTY(QAbstractSocket::SocketState networkState READ networkState NOTIFY networkStateChanged)
    Q_PROPERTY(QAbstractSocket::SocketError networkError READ networkError NOTIFY networkErrorOccurred)

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
    explicit CoreInterface(QObject *parent = nullptr);
    ~CoreInterface();

private:
    void initDatabase();
    void initDiscovery();
    void initNetwork();


private:
    void test();


public:
    Q_INVOKABLE void discoverySearch();

    Q_INVOKABLE void networkConnectToDevice(const QString &host);
    Q_INVOKABLE void networkDisconnectFromDevice();

private:
    void onDiscoveryClientWorkerStartedSearch();
    void onDiscoveryClientWorkerFinishedSearch();
    void onDiscoveryClientWorkerHostFound(const QString &host);

    void onNetworkClientWorkerHostChanged(const QString &host);
    void onNetworkClientWorkerConnected();
    void onNetworkClientWorkerDisconnected();
    void onNetworkClientWorkerStateChanged(QAbstractSocket::SocketState socketState);
    void onNetworkClientWorkerErrorOccurred(QAbstractSocket::SocketError socketError);

public:
    bool discoveryWorking() const;

    QString networkHost() const;
    QAbstractSocket::SocketState networkState() const;
    QAbstractSocket::SocketError networkError() const;

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

private:
    void setDiscoveryWorking(bool working);

    void setMessageClock(const asdc::proto::Clock &message, const QDateTime &messageReceivedAt);
    void setMessageConfiguration(const asdc::proto::Configuration &message, const QDateTime &messageReceivedAt);
    void setMessageError(const asdc::proto::Error &message, const QDateTime &messageReceivedAt);
    void setMessageFilter(const asdc::proto::Filter &message, const QDateTime &messageReceivedAt);
    void setMessageInformation(const asdc::proto::Information &message, const QDateTime &messageReceivedAt);
    void setMessageLive(const asdc::proto::Live &message, const QDateTime &messageReceivedAt);
    void setMessageOnzenLive(const asdc::proto::OnzenLive &message, const QDateTime &messageReceivedAt);
    void setMessageOnzenSettings(const asdc::proto::OnzenSettings &message, const QDateTime &messageReceivedAt);
    void setMessagePeak(const asdc::proto::Peak &message, const QDateTime &messageReceivedAt);
    void setMessagePeripheral(const asdc::proto::Peripheral &message, const QDateTime &messageReceivedAt);
    void setMessageSettings(const asdc::proto::Settings &message, const QDateTime &messageReceivedAt);

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
    bool isCommandIntValid(const QString &name, qint32 value);
    void sendCommand(const QString &name, const QVariant &value);

public:
    Q_INVOKABLE void commandSetTemperatureSetpointFahrenheit(qint32 value);
    Q_INVOKABLE void commandSetPump1(qint32 value);
    Q_INVOKABLE void commandSetPump2(qint32 value);
    Q_INVOKABLE void commandSetPump3(qint32 value);
    Q_INVOKABLE void commandSetPump4(qint32 value);
    Q_INVOKABLE void commandSetPump5(qint32 value);
    Q_INVOKABLE void commandSetBlower1(qint32 value);
    Q_INVOKABLE void commandSetBlower2(qint32 value);
    Q_INVOKABLE void commandSetLights(bool value);
    Q_INVOKABLE void commandSetStereo(bool value);
    Q_INVOKABLE void commandSetFilter(bool value);
    Q_INVOKABLE void commandSetOnzen(bool value);
    Q_INVOKABLE void commandSetOzone(bool value);
    Q_INVOKABLE void commandSetExhaustFan(bool value);
    Q_INVOKABLE void commandSetSaunaState(qint32 value);
    Q_INVOKABLE void commandSetSaunaTimeLeft(qint32 value);
    Q_INVOKABLE void commandSetAllOn(bool value);
    Q_INVOKABLE void commandSetFogger(bool value);
    Q_INVOKABLE void commandSetSpaboyBoost(bool value);
    Q_INVOKABLE void commandSetPackReset(bool value);
    Q_INVOKABLE void commandSetLogDump(bool value);
    Q_INVOKABLE void commandSetSds(bool value);
    Q_INVOKABLE void commandSetYess(bool value);

private:
    asdc::db::DatabaseClient *m_databaseClient = nullptr;
    QThread *m_networkClientWorkerThread = nullptr;
    QThread *m_discoveryClientWorkerThread = nullptr;

    bool m_discoveryWorking = false;

    QString m_networkHost;
    QAbstractSocket::SocketState m_networkState = QAbstractSocket::SocketState::UnconnectedState;
    QAbstractSocket::SocketError m_networkError = QAbstractSocket::SocketError::UnknownSocketError;

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
    void discoveryClientWorkerSearch();

    void networkClientWorkerConnectToDevice(const QString &host);
    void networkClientWorkerDisconnectFromDevice();
    void networkClientWorkerQueueMessage(const asdc::net::MessageType &messageType);
    void networkClientWorkerQueueCommand(const QString &name, const QVariant &value);

signals:
    void discoveryStartedSearch();
    void discoveryFinishedSearch();
    void discoveryWorkingChanged();
    void discoveryHostFound(const QString &host);

    void networkHostChanged();
    void networkConnected();
    void networkDisconnected();
    void networkStateChanged();
    void networkErrorOccurred();

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

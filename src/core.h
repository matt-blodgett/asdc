#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QAbstractSocket>

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


namespace asdc::network {
    enum class MessageType;
    class Client;
}

namespace asdc::core {

class NetworkClientWorker : public QObject {
    Q_OBJECT

public:
    explicit NetworkClientWorker(QObject *parent = nullptr);

public slots:
    void connectClient(const QString &host);
    void disconnectClient();
    void queueMessage(const asdc::network::MessageType &messageType);

private:
    asdc::network::Client *m_networkClient;

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

    Q_PROPERTY(QString clientHost READ getClientHost NOTIFY clientHostChanged)
    Q_PROPERTY(QAbstractSocket::SocketState clientState READ getClientState NOTIFY clientStateChanged)
    Q_PROPERTY(QAbstractSocket::SocketError clientError READ getClientError NOTIFY clientErrorOccurred)

    Q_PROPERTY(asdc::proto::Clock messageClock READ getMessageClock NOTIFY messageClockChanged)
    Q_PROPERTY(asdc::proto::Configuration messageConfiguration READ getMessageConfiguration NOTIFY messageConfigurationChanged)
    Q_PROPERTY(asdc::proto::Error messageError READ getMessageError NOTIFY messageErrorChanged)
    Q_PROPERTY(asdc::proto::Filter messageFilter READ getMessageFilter NOTIFY messageFilterChanged)
    Q_PROPERTY(asdc::proto::Information messageInformation READ getMessageInformation NOTIFY messageInformationChanged)
    Q_PROPERTY(asdc::proto::Live messageLive READ getMessageLive NOTIFY messageLiveChanged)
    Q_PROPERTY(asdc::proto::OnzenLive messageOnzenLive READ getMessageOnzenLive NOTIFY messageOnzenLiveChanged)
    Q_PROPERTY(asdc::proto::OnzenSettings messageOnzenSettings READ getMessageOnzenSettings NOTIFY messageOnzenSettingsChanged)
    Q_PROPERTY(asdc::proto::Peak messagePeak READ getMessagePeak NOTIFY messagePeakChanged)
    Q_PROPERTY(asdc::proto::Peripheral messagePeripheral READ getMessagePeripheral NOTIFY messagePeripheralChanged)
    Q_PROPERTY(asdc::proto::Settings messageSettings READ getMessageSettings NOTIFY messageSettingsChanged)

public:
    explicit Core(QObject *parent = nullptr);
    ~Core();

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
    QString getClientHost() const;
    QAbstractSocket::SocketState getClientState() const;
    QAbstractSocket::SocketError getClientError() const;

    asdc::proto::Clock getMessageClock() const;
    asdc::proto::Configuration getMessageConfiguration() const;
    asdc::proto::Error getMessageError() const;
    asdc::proto::Filter getMessageFilter() const;
    asdc::proto::Information getMessageInformation() const;
    asdc::proto::Live getMessageLive() const;
    asdc::proto::OnzenLive getMessageOnzenLive() const;
    asdc::proto::OnzenSettings getMessageOnzenSettings() const;
    asdc::proto::Peak getMessagePeak() const;
    asdc::proto::Peripheral getMessagePeripheral() const;
    asdc::proto::Settings getMessageSettings() const;

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
    QThread *m_workerThread;

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

signals:
    void workerClientConnect(const QString &host);
    void workerClientDisconnect();
    void workerClientQueueMessage(const asdc::network::MessageType &messageType);

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

#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include <QObject>
#include <QAbstractSocket>
#include <QtProtobuf/QProtobufSerializer>

QT_FORWARD_DECLARE_CLASS(QTcpSocket)

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
Q_NAMESPACE


enum class MessageType : qint32 {
    LIVE = 0,
    COMMAND = 1,
    SETTINGS = 2,
    CONFIGURATION = 3,
    PEAK = 4,
    CLOCK = 5,
    INFORMATION = 6,
    ERROR = 7,
    FIRMWARE = 8,
    ROUTER = 9,
    HEARTBEAT = 10,
    FILTERS = 13,
    PERIPHERAL = 16,
    ONZEN_LIVE = 48,
    ONZEN_COMMAND = 49,
    ONZEN_SETTINGS = 50,
    MOBILE_AUTHENTICATE = 80,
    MOBILE_SPA = 81,
    MOBILE_AVAILABLE_SPAS = 82,
    MOBILE_ASSOCIATE_ACK = 83,
    MOBILE_SPA_REGISTRATION = 84,
    MOBILE_WIFI_DETAILS = 85,
    lpc_live = 112,
    lpc_command = 113,
    lpc_info = 114,
    lpc_config = 115,
    lpc_preferences = 116,
    lpc_lights = 117,
    lpc_schedule = 118,
    lpc_peak_devices = 119,
    lpc_clock = 120,
    lpc_error = 121,
    lpc_measurements = 122,
    lpc_diagnostic_command = 123,
    lpc_power = 124,
    lpc_enabled_devices = 125,
    reset = 127,
    lpc_android_topside_info = 144,
    firmware_success = 194,
    firmware_failure = 195,
    firmware_started = 196,
};
Q_ENUM_NS(MessageType)


struct Header {
    QByteArray preamble() const;

    quint8 skipped[4] = {0, 0, 0, 0};
    quint8 checksum[4] = {0, 0, 0, 0};

    quint32 counter = 0;
    quint32 unused = 0;

    quint16 messageType = 0;
    quint16 length = 0;

    QByteArray payload;

    bool isValid = false;

    void unpack(const QByteArray &data);

    Header(const QByteArray &data = QByteArray());
};


class NetworkClient : public QObject
{
    Q_OBJECT

public:
    explicit NetworkClient(QObject *parent = nullptr);
    ~NetworkClient();

public:
    bool connectToDevice(const QString &host, const quint16 &port = 65534);
    void disconnectFromDevice();

    bool isConnected() const;

    QString host() const;
    quint16 port() const;

    QAbstractSocket::SocketState state() const;
    QAbstractSocket::SocketError error() const;

public:
    bool writePacket(const MessageType &messageType, const QByteArray &payload = QByteArray());

private:
    void readAndParseData();

private:
    QString m_host;
    quint16 m_port;
    QTcpSocket *m_socket;

signals:
    void hostChanged(const QString &host);
    void portChanged(quint16 port);
    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState socketState);
    void errorOccurred(QAbstractSocket::SocketError socketError);
    void headerReceived(const asdc::net::Header &header);
};

class NetworkClientWorker : public QObject
{
    Q_OBJECT

public:
    explicit NetworkClientWorker(QObject *parent = nullptr);

public slots:
    void connectToDevice(const QString &host);
    void disconnectFromDevice();

    void queueMessage(const MessageType &messageType);
    void queueCommand(const QString &name, const QVariant &value);

private:
    bool serializerHasError();
    void onHeaderReceived(const Header &header);

private:
    NetworkClient *m_networkClient;
    QProtobufSerializer m_serializer;

signals:
    void hostChanged(const QString &host);
    void portChanged(quint16 port);
    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState socketState);
    void errorOccurred(QAbstractSocket::SocketError socketError);

    void receivedMessageClock(const asdc::proto::Clock &message, const QDateTime &messageReceivedAt);
    void receivedMessageConfiguration(const asdc::proto::Configuration &message, const QDateTime &messageReceivedAt);
    void receivedMessageError(const asdc::proto::Error &message, const QDateTime &messageReceivedAt);
    void receivedMessageFilter(const asdc::proto::Filter &message, const QDateTime &messageReceivedAt);
    void receivedMessageInformation(const asdc::proto::Information &message, const QDateTime &messageReceivedAt);
    void receivedMessageLive(const asdc::proto::Live &message, const QDateTime &messageReceivedAt);
    void receivedMessageOnzenLive(const asdc::proto::OnzenLive &message, const QDateTime &messageReceivedAt);
    void receivedMessageOnzenSettings(const asdc::proto::OnzenSettings &message, const QDateTime &messageReceivedAt);
    void receivedMessagePeak(const asdc::proto::Peak &message, const QDateTime &messageReceivedAt);
    void receivedMessagePeripheral(const asdc::proto::Peripheral &message, const QDateTime &messageReceivedAt);
    void receivedMessageSettings(const asdc::proto::Settings &message, const QDateTime &messageReceivedAt);
};

};  // namespace asdc::net

#endif // NET_CLIENT_H

#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QtProtobuf/QProtobufSerializer>


namespace asdc::network {
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


class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    ~Client();

    bool connectToDevice(const QString &host, const quint16 &port = 65534);
    void disconnectFromDevice();

    bool isConnected() const;

    QString host() const;
    quint16 port() const;

    QAbstractSocket::SocketState state() const;
    QAbstractSocket::SocketError error() const;

    void decodeOne(const QByteArray &data);
    QByteArray requestMessage(const MessageType &messageType);
    QVector<Header> parseResponse(const QByteArray &data);

private:
    void checkSerializerError();

public:
    template<typename M>
    M getMessage(const MessageType &messageType){
        M message;
        QByteArray data = requestMessage(messageType);
        QVector<Header> headers = parseResponse(data);
        for (int i = 0; i < headers.length(); i++) {
            Header header = headers.at(i);
            if (header.messageType == static_cast<qint32>(messageType)) {
                message.deserialize(&m_serializer, header.payload);
                checkSerializerError();
                return message;
            }
        }
        return message;
    }

private:
    QString m_host;
    quint16 m_port;
    QTcpSocket *m_socket;
    QProtobufSerializer m_serializer;

signals:
    void hostChanged(const QString &host);
    void portChanged(const quint16 &port);
    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState socketState);
    void errorOccurred(QAbstractSocket::SocketError socketError);
};

};  // namespace asdc::network

#endif // CLIENT_H

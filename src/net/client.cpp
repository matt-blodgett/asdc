#include "client.h"

#include <QTcpSocket>

#include <QDateTime>

#include "packet.h"

#include "asdc/proto/Command.qpb.h"


#include <QDebug>




void printHex(const QByteArray &data) {
    QStringList hexBytes;
    for (unsigned char c : data) {
        hexBytes << QString("%1").arg(c, 2, 16, QLatin1Char('0')).toUpper();
    }
    qDebug().noquote() << hexBytes.join(' ');
}


namespace asdc::net {


QByteArray Header::preamble() const {
    return QByteArray::fromHex("ABAD1D3A");
}
void Header::unpack(const QByteArray &data) {
    if (data.size() < HEADER_SIZE) {
        isValid = false;
        qWarning()
            << "error decoding:"
            << "received" << data.size() << "bytes;"
            << "expecting at least" << HEADER_SIZE << "bytes";
        return;
    }

    if (data.mid(0, 4) != preamble()) {
        isValid = false;
        qWarning()
            << "error decoding:"
            << "data did not start with correct preamble";
        return;
    }

    // data is a pointer to at least 20 bytes
    quint8 b0 = data[4];
    quint8 b1 = data[5];
    quint8 b2 = data[6];
    quint8 b3 = data[7];

    // Read big-endian 32-bit unsigned ints
    quint32 i0 = (data[8] << 24) | (data[9] << 16) | (data[10] << 8) | data[11];
    quint32 i1 = (data[12] << 24) | (data[13] << 16) | (data[14] << 8) | data[15];

    // Read big-endian 16-bit unsigned shorts
    quint16 h0 = (data[16] << 8) | data[17];
    quint16 h1 = (data[18] << 8) | data[19];

    skipped[0] = data[0];
    skipped[1] = data[1];
    skipped[2] = data[2];
    skipped[3] = data[3];

    checksum[0] = b0;
    checksum[1] = b1;
    checksum[2] = b2;
    checksum[3] = b3;

    counter = i0;
    unused = i1;

    messageType = h0;
    length = h1;

    payload = data.mid(HEADER_SIZE, length);

    isValid = true;
}
Header::Header(const QByteArray &data) {
    if (!data.isEmpty()) {
        unpack(data);
    }
}


NetworkClient::NetworkClient(QObject *parent)
    : QObject{parent}
{
    m_socket = new QTcpSocket(this);

    connect(m_socket, &QTcpSocket::connected, this, &NetworkClient::connected);
    connect(m_socket, &QTcpSocket::disconnected, this, &NetworkClient::disconnected);
    connect(m_socket, &QTcpSocket::stateChanged, this, &NetworkClient::stateChanged);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &NetworkClient::errorOccurred);
    connect(m_socket, &QTcpSocket::readyRead, this, &NetworkClient::readAndParseData);
}
NetworkClient::~NetworkClient() {
    disconnectFromDevice();
}

bool NetworkClient::connectToDevice(const QString &host, const quint16 &port) {
    qDebug() << "connecting to" << host << "on port" << port << "...";

    qDebug() << "current state:" << m_socket->state();
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        qDebug() << "attempting to disconnect";
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            qDebug() << "waiting to disconnect";
            if (!m_socket->waitForDisconnected(3000)) {
                qWarning() << "still not disconnected; exiting";
                return false;
            } else {
                qDebug() << "disconnected";
            }
        }
    }

    if (m_host != host) {
        m_host = host;
        emit hostChanged(m_host);
    }
    if (m_port != port) {
        m_port = port;
        emit portChanged(m_port);
    }

    m_socket->connectToHost(host, port);
    if (!m_socket->waitForConnected(3000)) {
        qWarning() << "error: " << m_socket->errorString();
    }

    return isConnected();
}
void NetworkClient::disconnectFromDevice() {
    if (m_socket->state() == QAbstractSocket::UnconnectedState) {
        qDebug() << "already disconnected";
        return;
    }
    qDebug() << "disconnecting from host";

    m_socket->disconnectFromHost();
    if (m_socket->state() != QAbstractSocket::UnconnectedState) {
        qDebug() << "waiting to disconnect";
        if (!m_socket->waitForDisconnected(3000)) {
            qWarning() << "still not disconnected; exiting";
            return;
        }
    }

    qDebug() << "disconnected from host";
}

bool NetworkClient::isConnected() const {
    return m_socket->state() == QAbstractSocket::ConnectedState;
}
QString NetworkClient::host() const {
    return m_host;
    // return m_socket->localAddress().toString();
}
quint16 NetworkClient::port() const {
    return m_port;
    // return m_socket->localPort();
}

QAbstractSocket::SocketState NetworkClient::state() const {
    return m_socket->state();
}
QAbstractSocket::SocketError NetworkClient::error() const {
    return m_socket->error();
}

// void NetworkClient::decodeOne(const QByteArray &data) {
//     Header header(data);
//     if (!header.isValid) {
//         return;
//     }
//     qDebug() << "skipped =" << header.skipped;
//     qDebug() << "checksum =" << header.checksum;
//     qDebug() << "counter =" << header.counter;
//     qDebug() << "unused =" << header.unused;
//     qDebug() << "messageType =" << header.messageType;
//     qDebug() << "length =" << header.length;

//     QByteArray payload = data.mid(HEADER_SIZE, header.length);
//     QByteArray remainder = data.mid(HEADER_SIZE + header.length, -1);
//     qDebug() << "payload =" << payload;
//     qDebug() << "remainder =" << remainder;
// }
bool NetworkClient::writePacket(const MessageType &messageType, const QByteArray &payload) {
    if (!isConnected()) {
        qWarning() << "socket not connected!";
        return false;
    }

    Packet packet(static_cast<qint16>(messageType), payload);
    QByteArray packetBytes = packet.serialize();

    qDebug() << "writing message type" << messageType;

    m_socket->write(packetBytes);
    if (!m_socket->waitForBytesWritten(3000)) {
        qWarning() << "write timeout!";
        return false;
    }

    return true;
}

void NetworkClient::readAndParseData() {
    qDebug() << "data ready to read from host:" << m_socket->bytesAvailable() << "bytes";

    QByteArray data = m_socket->read(2048);

    while (!data.isEmpty()) {
        Header header(data);

        if (!header.isValid) {
            break;
        }

        MessageType messageType = static_cast<MessageType>(header.messageType);
        if (messageType != MessageType::HEARTBEAT) {
            qDebug() << "got header message type:" << messageType;
            emit headerReceived(header);
        }

        QByteArray remainder = data.mid(HEADER_SIZE + header.length, -1);
        data = remainder;
    }
}



NetworkClientWorker::NetworkClientWorker(QObject *parent)
    : QObject{parent}
{
    m_networkClient = new NetworkClient(this);

    connect(m_networkClient, &NetworkClient::hostChanged, this, &NetworkClientWorker::hostChanged);
    connect(m_networkClient, &NetworkClient::portChanged, this, &NetworkClientWorker::portChanged);
    connect(m_networkClient, &NetworkClient::connected, this, &NetworkClientWorker::connected);
    connect(m_networkClient, &NetworkClient::disconnected, this, &NetworkClientWorker::disconnected);
    connect(m_networkClient, &NetworkClient::stateChanged, this, &NetworkClientWorker::stateChanged);
    connect(m_networkClient, &NetworkClient::errorOccurred, this, &NetworkClientWorker::errorOccurred);
    connect(m_networkClient, &NetworkClient::headerReceived, this, &NetworkClientWorker::onHeaderReceived);
};

void NetworkClientWorker::connectToDevice(const QString &host) {
    m_networkClient->connectToDevice(host);
}
void NetworkClientWorker::disconnectFromDevice() {
    m_networkClient->disconnectFromDevice();
}

void NetworkClientWorker::queueMessage(const MessageType &messageType) {
    qDebug() << "requesting message" << messageType;
    m_networkClient->writePacket(messageType);
}
void NetworkClientWorker::queueCommand(const QString &name, const QVariant &value) {
    asdc::proto::Command command;
    command.setProperty(name, value);

    QByteArray payload = command.serialize(&m_serializer);
    if (serializerHasError()) {
        return;
    }

    qDebug() << "requesting command:" << name << "->" << value;
    m_networkClient->writePacket(MessageType::COMMAND, payload);
}

bool NetworkClientWorker::serializerHasError() {
    if (m_serializer.lastError() != QAbstractProtobufSerializer::Error::None) {
        qWarning().nospace()
        << "Unable to deserialize ("
        << qToUnderlying(m_serializer.lastError()) << ")"
        << m_serializer.lastErrorString();
        return true;
    }
    return false;
}
void NetworkClientWorker::onHeaderReceived(const Header &header) {
    MessageType messageType = static_cast<MessageType>(header.messageType);

    if (messageType == MessageType::CLOCK) {
        asdc::proto::Clock message;
        message.deserialize(&m_serializer, header.payload);
        if (!serializerHasError()) {
            emit receivedMessageClock(message, QDateTime::currentDateTime());
        }
    } else if (messageType == MessageType::CONFIGURATION) {
        asdc::proto::Configuration message;
        message.deserialize(&m_serializer, header.payload);
        if (!serializerHasError()) {
            emit receivedMessageConfiguration(message, QDateTime::currentDateTime());
        }
    } else if (messageType == MessageType::ERROR) {
        asdc::proto::Error message;
        message.deserialize(&m_serializer, header.payload);
        if (!serializerHasError()) {
            emit receivedMessageError(message, QDateTime::currentDateTime());
        }
    } else if (messageType == MessageType::FILTERS) {
        asdc::proto::Filter message;
        message.deserialize(&m_serializer, header.payload);
        if (!serializerHasError()) {
            emit receivedMessageFilter(message, QDateTime::currentDateTime());
        }
    } else if (messageType == MessageType::INFORMATION) {
        asdc::proto::Information message;
        message.deserialize(&m_serializer, header.payload);
        if (!serializerHasError()) {
            emit receivedMessageInformation(message, QDateTime::currentDateTime());
        }
    } else if (messageType == MessageType::LIVE) {
        asdc::proto::Live message;
        message.deserialize(&m_serializer, header.payload);
        if (!serializerHasError()) {
            emit receivedMessageLive(message, QDateTime::currentDateTime());
        }
    } else if (messageType == MessageType::ONZEN_LIVE) {
        asdc::proto::OnzenLive message;
        message.deserialize(&m_serializer, header.payload);
        if (!serializerHasError()) {
            emit receivedMessageOnzenLive(message, QDateTime::currentDateTime());
        }
    } else if (messageType == MessageType::ONZEN_SETTINGS) {
        asdc::proto::OnzenSettings message;
        message.deserialize(&m_serializer, header.payload);
        if (!serializerHasError()) {
            emit receivedMessageOnzenSettings(message, QDateTime::currentDateTime());
        }
    } else if (messageType == MessageType::PEAK) {
        asdc::proto::Peak message;
        message.deserialize(&m_serializer, header.payload);
        if (!serializerHasError()) {
            emit receivedMessagePeak(message, QDateTime::currentDateTime());
        }
    } else if (messageType == MessageType::PERIPHERAL) {
        asdc::proto::Peripheral message;
        message.deserialize(&m_serializer, header.payload);
        if (!serializerHasError()) {
            emit receivedMessagePeripheral(message, QDateTime::currentDateTime());
        }
    } else if (messageType == MessageType::SETTINGS) {
        asdc::proto::Settings message;
        message.deserialize(&m_serializer, header.payload);
        if (!serializerHasError()) {
            emit receivedMessageSettings(message, QDateTime::currentDateTime());
        }
    }
}


};  // namespace asdc::net


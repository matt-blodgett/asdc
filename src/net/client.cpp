#include "client.h"

#include <QDebug>

#include "packet.h"


static const qint32 HEADER_SIZE = 20;


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

void NetworkClient::decodeOne(const QByteArray &data) {
    Header header(data);
    if (!header.isValid) {
        return;
    }
    qDebug() << "skipped =" << header.skipped;
    qDebug() << "checksum =" << header.checksum;
    qDebug() << "counter =" << header.counter;
    qDebug() << "unused =" << header.unused;
    qDebug() << "messageType =" << header.messageType;
    qDebug() << "length =" << header.length;

    QByteArray payload = data.mid(HEADER_SIZE, header.length);
    QByteArray remainder = data.mid(HEADER_SIZE + header.length, -1);
    qDebug() << "payload =" << payload;
    qDebug() << "remainder =" << remainder;
}
QByteArray NetworkClient::requestMessage(const MessageType &messageType) {
    if (!isConnected()) {
        qWarning() << "socket not connected!";
        return {};
    }

    Packet packet(static_cast<qint32>(messageType));
    QByteArray packetBytes = packet.serialize();

    qDebug() << "writing message type" << messageType;

    // Write requested message
    m_socket->write(packetBytes);
    if (!m_socket->waitForBytesWritten(3000)) {
        qWarning() << "write timeout!";
        return {};
    }

    // Wait for response
    if (!m_socket->waitForReadyRead(3000)) {
        qWarning() << "read timeout!";
        return {};
    }

    qDebug() << "reading response";

    QByteArray data = m_socket->read(1024);

    return data;
}
QVector<Header> NetworkClient::parseResponse(const QByteArray &responseData) {

    QVector<Header> headers;

    QByteArray data(responseData);

    while (!data.isEmpty()) {
        Header h(data);

        if (!h.isValid) {
            break;
        }

        headers.append(h);

        qDebug() << "header message type:" << static_cast<MessageType>(h.messageType);

        QByteArray remainder = data.mid(HEADER_SIZE + h.length, -1);
        data = remainder;
    }

    qDebug() << "got" << headers.length() << "messages";
    return headers;
}
void NetworkClient::checkSerializerError() {
    if (m_serializer.lastError() != QAbstractProtobufSerializer::Error::None) {
        qWarning().nospace()
        << "Unable to deserialize ("
        << qToUnderlying(m_serializer.lastError()) << ")"
        << m_serializer.lastErrorString();
    }
}

};  // namespace asdc::net


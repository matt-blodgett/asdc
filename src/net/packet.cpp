#include "packet.h"

#include <QtZlib/zlib.h>

#include <QDebug>

#include "bytebuffer.h"


unsigned long calculateCrc32(const unsigned char* data, unsigned int length) {
    unsigned long crc = crc32(0L, Z_NULL, 0);
    return crc32(crc, data, length);
}


namespace asdc::net {


Packet::Packet(qint16 type, const QByteArray& payload) {
    m_sequenceNumber = 0;
    m_optional = 0;
    m_type = type;
    m_size = payload.size();
    m_payload = payload;
    m_checksum = 0;
}

bool Packet::isChecksumValid() {
    ByteBuffer buffer(m_payload.size() + HEADER_SIZE);
    buffer.putInt(-1414717974);
    buffer.putInt(0);
    buffer.putInt(m_sequenceNumber);
    buffer.putInt(m_optional);
    buffer.putShort(m_type);
    buffer.putShort(m_size);
    buffer.putBytesAt(HEADER_SIZE, m_payload);

    QByteArray data = buffer.getBuffer();

    std::vector<unsigned char> ucharVector(data.size());
    for (int i = 0; i < data.size(); ++i) {
        ucharVector[i] = static_cast<unsigned char>(data.at(i));
    }
    quint32 crc32 = calculateCrc32(ucharVector.data(), data.size());
    return (crc32 & 0xFFFFFFFF) == crc32;
}

QByteArray Packet::serialize() {
    ByteBuffer buffer(m_size + HEADER_SIZE);
    buffer.putInt(-1414718150);
    buffer.putInt(0);
    buffer.putInt(m_sequenceNumber);
    buffer.putInt(m_optional);
    buffer.putShort(m_type);
    buffer.putShort(m_size);
    buffer.putBytesAt(HEADER_SIZE, m_payload);

    QByteArray data = buffer.getBuffer();

    std::vector<unsigned char> ucharVector(data.size());
    for (int i = 0; i < data.size(); ++i) {
        ucharVector[i] = static_cast<unsigned char>(data.at(i));
    }
    m_checksum = calculateCrc32(ucharVector.data(), data.size());
    buffer.putIntAt(4, m_checksum);

    return buffer.getBuffer();
}

};  // namespace asdc::net

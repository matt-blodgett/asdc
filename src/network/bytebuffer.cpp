#include "bytebuffer.h"

#include <QBuffer>


// Packs a qint32 into a 4-byte array in big-endian order
QByteArray pack_qint32_be(qint32 value) {
    QByteArray bytes;
    bytes.resize(4);
    bytes[0] = (value >> 24) & 0xFF;
    bytes[1] = (value >> 16) & 0xFF;
    bytes[2] = (value >> 8) & 0xFF;
    bytes[3] = value & 0xFF;
    return bytes;
}

namespace asdc::network {

ByteBuffer::ByteBuffer(int capacity) : m_buffer(capacity, 0), m_stream(&m_buffer, QIODevice::ReadWrite) {}

void ByteBuffer::putInt(qint32 value) { m_stream << value; }
void ByteBuffer::putShort(qint16 value) { m_stream << value; }
void ByteBuffer::putBytes(const QByteArray& value) { m_buffer.append(value); }
qint32 ByteBuffer::getInt() { qint32 v; m_stream >> v; return v; }
qint16 ByteBuffer::getShort() { qint16 v; m_stream >> v; return v; }
QByteArray ByteBuffer::getBytes(int length) { return m_buffer.mid(m_stream.device()->pos(), length); }
void ByteBuffer::flip() { m_stream.device()->seek(0); }
void ByteBuffer::clear() { m_buffer.clear(); m_stream.device()->seek(0); }
QByteArray& ByteBuffer::getBuffer() { return m_buffer; }

void ByteBuffer::putIntAt(qint64 pos, qint32 value) {
    QByteArray packed = pack_qint32_be(value);
    qint64 currentPos = m_stream.device()->pos();
    m_stream.device()->seek(pos);
    m_stream.device()->write(packed);
    m_stream.device()->seek(currentPos);
}

};  // namespace asdc::network

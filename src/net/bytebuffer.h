#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#include <QByteArray>
#include <QDataStream>


namespace asdc::net {

class ByteBuffer {

public:
    ByteBuffer(int capacity = 0);

    void putInt(qint32 value);
    void putShort(qint16 value);
    void putBytes(const QByteArray& value);
    qint32 getInt();
    qint16 getShort();
    QByteArray getBytes(int length);
    void flip();
    void clear();
    QByteArray& getBuffer();

    void putIntAt(qint64 pos, qint32 value);

private:
    QByteArray m_buffer;
    QDataStream m_stream;
};

};  // namespace asdc::net

#endif // BYTEBUFFER_H

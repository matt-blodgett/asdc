#ifndef PACKET_H
#define PACKET_H

#include <QByteArray>


namespace asdc::network {

class Packet {

public:
    Packet(qint16 type = 0, const QByteArray& payload = QByteArray());

    bool isChecksumValid();
    QByteArray serialize();

private:
    qint32 m_sequenceNumber;
    qint32 m_optional;
    qint16 m_type;
    qint16 m_size;
    QByteArray m_payload;
    quint32 m_checksum;
};

};  // namespace network::asdc

#endif // PACKET_H

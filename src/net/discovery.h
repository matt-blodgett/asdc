#ifndef DISCOVERY_H
#define DISCOVERY_H

#include <QObject>


namespace asdc::net {


class DiscoveryClient : public QObject
{
    Q_OBJECT

public:
    explicit DiscoveryClient(QObject *parent = nullptr);

    void test();

    void setIpAndMask(QString ipAddress, int netmaskCidr);


    // Returns a list of IPs (as strings) that responded with the expected prefix.
    // timeoutMs applies per-host probe; maxWorkers limits concurrency.
    QStringList search(int timeoutMs = 1000, int maxWorkers = 50) const;

    static constexpr quint16 QUERY_PORT    = 9131;
    static constexpr quint16 RESPONSE_PORT = 33327;

private:
    static bool udpProbe(const QString& host,
                         const QByteArray& query,
                         quint16 queryPort,
                         const QByteArray& responsePrefix,
                         int timeoutMs);

    QStringList enumerateHosts() const;  // hosts in subnet

private:
    QString m_ip;
    int m_prefix;


};

};  // namespace asdc::net

#endif // DISCOVERY_H

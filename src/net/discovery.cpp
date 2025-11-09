#include "discovery.h"

#include <QUdpSocket>
#include <QHostAddress>
#include <QNetworkProxy>
#include <QNetworkInterface>
#include <QElapsedTimer>
#include <QThreadPool>
#include <QRunnable>
#include <QMutex>
#include <QMutexLocker>
#include <algorithm>

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(netLog)
Q_DECLARE_LOGGING_CATEGORY(workerLog)


namespace asdc::net {


// Returns the local IPv4 address that would be used for the default route
static QString localIPv4ViaDefaultRoute()
{
    QUdpSocket sock;
    sock.setProxy(QNetworkProxy::NoProxy); // avoid system proxies interfering

    // Doesn’t need to be reachable; it just forces the kernel to pick a route.
    const QHostAddress dummyDst(QStringLiteral("10.254.254.254"));

    // For UDP, connectToHost() is immediate (no handshake); it sets the peer
    // and causes the kernel to choose a local address/port.
    sock.connectToHost(dummyDst, /*port*/ 1);
    sock.waitForConnected(1); // returns immediately for UDP; safe to ignore result

    QHostAddress local = sock.localAddress();
    if (!local.isNull()
        && local.protocol() == QAbstractSocket::IPv4Protocol
        && !local.isLoopback())
    {
        return local.toString();
    }

    // Fallback: pick the first non-loopback IPv4 from interfaces
    const auto addrs = QNetworkInterface::allAddresses();
    for (const QHostAddress &addr : addrs) {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol && !addr.isLoopback())
            return addr.toString();
    }

    return QStringLiteral("127.0.0.1");
}


static inline QString ipv4ToString(quint32 h) {
    return QString::number((h >> 24) & 0xFF) + "." +
           QString::number((h >> 16) & 0xFF) + "." +
           QString::number((h >> 8)  & 0xFF) + "." +
           QString::number(h & 0xFF);
}

DiscoveryClient::DiscoveryClient(QObject *parent)
    : QObject{parent}
{

}

static inline QStringList enumerateHosts(const QString &ipAddress, int prefix)
{
    QHostAddress ip(ipAddress);
    quint32 ip4 = ip.toIPv4Address();
    if (ip4 == 0 && ipAddress != "0.0.0.0")
        return {}; // not IPv4

    // Build mask from CIDR (host byte order)
    quint32 mask = (prefix == 0) ? 0u : (0xFFFFFFFFu << (32 - prefix));
    quint32 network = ip4 & mask;
    quint32 broadcast = network | ~mask;

    quint32 start = (prefix <= 30) ? (network + 1) : network;
    quint32 end   = (prefix <= 30) ? (broadcast - 1) : broadcast;

    if (start > end)
        return {};

    QStringList out;
    out.reserve(int(end - start + 1));
    for (quint32 a = start; a <= end; ++a)
        out.push_back(ipv4ToString(a));
    return out;
}

static inline bool udpProbe(const QString &host,
                            const QByteArray &query,
                            quint16 queryPort,
                            const QByteArray &responsePrefix,
                            int timeoutMs)
{
    QUdpSocket sock;
    sock.setProxy(QNetworkProxy::NoProxy);

    // Bind to an ephemeral local port
    if (!sock.bind(QHostAddress::AnyIPv4, 0))
        return false;

    const QHostAddress dst(host);
    if (dst.isNull())
        return false;

    if (sock.writeDatagram(query, dst, queryPort) != query.size())
        return false;

    QElapsedTimer timer;
    timer.start();

    const QHostAddress expectedHost(host);

    while (timer.elapsed() < timeoutMs) {
        int remain = timeoutMs - int(timer.elapsed());
        if (!sock.waitForReadyRead(remain))
            break;

        while (sock.hasPendingDatagrams()) {
            QByteArray buf;
            buf.resize(int(sock.pendingDatagramSize()));
            QHostAddress sender;
            quint16 senderPort = 0;

            if (sock.readDatagram(buf.data(), buf.size(), &sender, &senderPort) >= 0) {
                // Match host and response prefix (ports optional; uncomment if needed)
                // if (senderPort != RESPONSE_PORT) continue;
                if (sender == expectedHost && buf.startsWith(responsePrefix))
                    return true;
            }
        }
    }
    return false;
}

QStringList DiscoveryClient::search(const QString &ipAddress,
                                    int netmaskCidr,
                                    int timeoutMs,
                                    int maxWorkers)
{
    const QByteArray QUERY = QByteArrayLiteral("Query,BlueFalls,");
    const QByteArray RESPONSE = QByteArrayLiteral("Response,BlueFalls,");
    const quint16 QUERY_PORT = 9131;
    const quint16 RESPONSE_PORT = 33327;

    qCDebug(netLog) << "initializing search";

    QString ipAddr(ipAddress);
    if (ipAddr == QString()) {
        qCDebug(netLog) << "no ip address supplied, getting local ipv4 default";
        ipAddr = localIPv4ViaDefaultRoute();
        qCDebug(netLog) << "using local ipv4 default route" << ipAddr;
    }

    qCInfo(netLog).nospace()
        << "scanning network:"
        << " ip supplied=" << ipAddress
        << ", ip fallback=" << ipAddr
        << ", netmask=" << netmaskCidr
        << ", timeout=" << timeoutMs
        << ", workers=" << maxWorkers;

    QStringList hosts = enumerateHosts(ipAddr, netmaskCidr);
    if (hosts.isEmpty()) {
        qCWarning(netLog) << "no hosts found to search; exiting";
        return {};
    }

    qCDebug(netLog)
        << "found" << hosts.length()
        << "hosts to query; starting udp probes";

    QStringList found;
    QMutex foundMutex;

    QThreadPool pool;
    pool.setMaxThreadCount(std::max(1, maxWorkers));

    foreach (const QString& h, hosts) {
        auto task = QRunnable::create([&, h]{
            if (udpProbe(h, QUERY, QUERY_PORT, RESPONSE, timeoutMs)) {
                QMutexLocker lock(&foundMutex);
                found.push_back(h);
            }
        });
        pool.start(task);
    }

    pool.waitForDone();

    qCInfo(netLog) << "discovered" << found.length() << "valid hosts";
    if (found.length() > 0) {
        qCInfo(netLog) << "discovered hosts:" << found;
    }

    // std::sort(found.begin(), found.end());
    return found;
}


DiscoveryClientWorker::DiscoveryClientWorker(QObject *parent)
    : QObject{parent}
{
    m_discoveryClient = new DiscoveryClient(this);
}

void DiscoveryClientWorker::search()
{
    qCDebug(workerLog) << "starting discovery search";

    m_hostsFound.clear();
    emit startedSearch();

    m_hostsFound = m_discoveryClient->search();

    qCDebug(workerLog) << "discovered" << m_hostsFound.length() << "valid hosts";
    if (m_hostsFound.length() > 0) {
        qCDebug(workerLog) << "discovered hosts:" << m_hostsFound;
    }

    foreach (const QString &host, m_hostsFound) {
        emit hostFound(host);
    }

    qCDebug(workerLog) << "finished discovery search";
    emit finishedSearch();
}


};  // namespace asdc::net

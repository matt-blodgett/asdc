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
    if (!local.isNull() &&
        local.protocol() == QAbstractSocket::IPv4Protocol &&
        !local.isLoopback())
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




// ---- helpers ----
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

void DiscoveryClient::test() {
    qDebug() << localIPv4ViaDefaultRoute();
}

void DiscoveryClient::setIpAndMask(QString ipAddress, int netmaskCidr) {
    m_ip = ipAddress;
    m_prefix = netmaskCidr;
}

QStringList DiscoveryClient::enumerateHosts() const
{
    QHostAddress ip(m_ip);
    quint32 ip4 = ip.toIPv4Address();        // host byte order
    if (ip4 == 0 && m_ip != "0.0.0.0")
        return {}; // not IPv4

    // Build mask from CIDR (host byte order)
    quint32 mask = (m_prefix == 0) ? 0u : (0xFFFFFFFFu << (32 - m_prefix));
    quint32 network = ip4 & mask;
    quint32 broadcast = network | ~mask;

    quint32 start = (m_prefix <= 30) ? (network + 1) : network;
    quint32 end   = (m_prefix <= 30) ? (broadcast - 1) : broadcast;

    if (start > end) return {};

    QStringList out;
    out.reserve(int(end - start + 1));
    for (quint32 a = start; a <= end; ++a)
        out.push_back(ipv4ToString(a));
    return out;
}

bool DiscoveryClient::udpProbe(const QString& host,
                             const QByteArray& query,
                             quint16 queryPort,
                             const QByteArray& responsePrefix,
                             int timeoutMs)
{
    QUdpSocket sock;
    sock.setProxy(QNetworkProxy::NoProxy);

    // Bind to an ephemeral local port (like Python's bind('', 0))
    if (!sock.bind(QHostAddress::AnyIPv4, 0))
        return false;

    // Send query
    const QHostAddress dst(host);
    if (dst.isNull()) return false;

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

QStringList DiscoveryClient::search(int timeoutMs, int maxWorkers) const
{
    const QByteArray QUERY    = QByteArrayLiteral("Query,BlueFalls,");
    const QByteArray RESPONSE = QByteArrayLiteral("Response,BlueFalls,");

    QStringList hosts = enumerateHosts();
    if (hosts.isEmpty())
        return {};

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

    std::sort(found.begin(), found.end());
    return found;
}


};


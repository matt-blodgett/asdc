#ifndef DISCOVERY_H
#define DISCOVERY_H

#include <QObject>


namespace asdc::net {


class DiscoveryClient : public QObject
{
    Q_OBJECT

public:
    explicit DiscoveryClient(QObject *parent = nullptr);

public:
    QStringList search(const QString &ipAddress = QString(),
                       int netmaskCidr = 24,
                       int timeoutMs = 1000,
                       int maxWorkers = 50);
};


class DiscoveryClientWorker : public QObject
{
    Q_OBJECT

public:
    explicit DiscoveryClientWorker(QObject *parent = nullptr);

public slots:
    void search();

private:
    DiscoveryClient *m_discoveryClient;
    QStringList m_hostsFound;

signals:
    void startedSearch();
    void finishedSearch();
    void hostFound(const QString &host);
};

};  // namespace asdc::net

#endif // DISCOVERY_H

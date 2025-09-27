#ifndef DB_CLIENT_H
#define DB_CLIENT_H

#include <QObject>
#include <QSqlDatabase>


QT_FORWARD_DECLARE_CLASS(QProtobufMessage)


namespace asdc::net {
enum class MessageType;
}

namespace asdc::db {


class DatabaseClient : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseClient(QObject *parent = nullptr);

    bool openDatabase();

private:
    bool validateSchema();
    void initializeSchema();
    void createSession();

public slots:
    void writeMessage(const asdc::net::MessageType &messageType, QProtobufMessage *message, const QDateTime &messageReceivedAt);

private:
    QSqlDatabase m_database;

    QVariant m_sessionId;

signals:
};


};  // namespace asdc::db

#endif // DB_CLIENT_H

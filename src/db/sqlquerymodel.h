#pragma once

#ifndef SQLQUERYMODEL_H
#define SQLQUERYMODEL_H

#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QHash>
#include <QByteArray>
#include <QSqlRecord>

#include <QSqlQuery>

#include <QDebug>


class SqlTableModel : public QSqlTableModel {
    Q_OBJECT
    Q_PROPERTY(QString table READ table WRITE setTableX NOTIFY tableChanged)
public:
    explicit SqlTableModel(QObject *parent = nullptr)
        : QSqlTableModel(parent) {}

    QString table() const { return m_table; }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
        if (section >= m_headers.length()) {
            return "";
        }
        return m_headers.at(section);
    }

public slots:
    void setTableX(const QString &table) {
        QSqlTableModel::setTable(table);
        select();

        QSqlQuery q(database());
        q.exec("PRAGMA table_info(message_live);");

        m_headers.clear();
        while (q.next()) {
            m_headers.append(q.value(1).toString());
        }

        emit layoutChanged();
        emit tableChanged();
    }

private:
    QString m_table;

    QStringList m_headers;

signals:
    void tableChanged();
};




class SqlQueryModel : public QSqlQueryModel {
    Q_OBJECT
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)
public:
    explicit SqlQueryModel(QObject *parent = nullptr)
        : QSqlQueryModel(parent) {}

    // Map roles to column names
    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        for (int i = 0; i < record().count(); ++i) {
            roles[Qt::UserRole + 1 + i] = record().fieldName(i).toUtf8();
        }
        return roles;
    }

    QVariant data(const QModelIndex &index, int role) const override {
        if (role < Qt::UserRole)
            return QSqlQueryModel::data(index, role);
        int col = role - Qt::UserRole - 1;
        QModelIndex modelIndex = this->index(index.row(), col);
        return QSqlQueryModel::data(modelIndex, Qt::DisplayRole);
    }

    // QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override{
    //     qDebug() << section;
    //     if (section == 0) {
    //         return "ID";
    //     } else if (section == 1) {
    //         return "Created At";
    //     } else if (section == 2) {
    //         return "Temperature";
    //     }
    //     return "";
    // }

    QString query() const { return m_query; }

public slots:
    void setQuery(const QString &q) {
        if (q == m_query) return;
        m_query = q;
        QSqlQueryModel::setQuery(q);
        // refresh role names after schema known
        emit layoutChanged();
        emit queryChanged();
    }

signals:
    void queryChanged();

private:
    QString m_query;
};

#endif // SQLQUERYMODEL_H

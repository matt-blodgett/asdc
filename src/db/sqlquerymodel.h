#pragma once

#ifndef SQLQUERYMODEL_H
#define SQLQUERYMODEL_H

#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QByteArray>
#include <QHash>
#include <QVariant>

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(dbLog)


class SqlTableModel : public QSqlTableModel
{
    Q_OBJECT

    Q_PROPERTY(QString table READ tableName WRITE setTable NOTIFY tableChanged)

    Q_PROPERTY(int sortColumn READ sortColumn NOTIFY sortChanged)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder NOTIFY sortChanged)

    Q_PROPERTY(QString genericFilterString MEMBER m_genericFilterString NOTIFY genericFilterStringChanged)

public:
    explicit SqlTableModel(QObject *parent = nullptr)
        : QSqlTableModel(parent) {}

    int sortColumn() const {
        return m_sortColumn;
    }
    Qt::SortOrder sortOrder() const {
        return m_sortOrder;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {        
        if (section < 0 || section >= m_headers.length()) {
            return "";
        }
        return m_headers.at(section);
    }

    Q_INVOKABLE QString columnFilterString(int column) const {
        if (column < 0 || column >= m_columnFilterStrings.length()) {
            return "";
        }
        return m_columnFilterStrings.at(column);
    }
    Q_INVOKABLE bool setColumnFilterString(int column, const QString &value) {
        if (column < 0 || column >= m_columnFilterStrings.length()) {
            return false;
        }
        m_columnFilterStrings.replace(column, value);
        return true;
    }

public:
    Q_INVOKABLE void setTable(const QString &tableName) override {
        QSqlTableModel::setTable(tableName);
        select();

        QSqlQuery query(database());
        QString sql = "PRAGMA table_info(" + tableName + ");";

        query.exec(sql);

        m_headers.clear();
        m_columnFilterStrings.clear();
        while (query.next()) {
            m_headers.append(query.value(1).toString());
            m_columnFilterStrings.append("");
        }

        m_genericFilterString = "";
        emit genericFilterStringChanged();

        emit layoutChanged();
        emit tableChanged();

        applySort(0, Qt::AscendingOrder);
    }

    Q_INVOKABLE void applySort(const int &column, const Qt::SortOrder &order) {
        if (column < 0 || column >= m_headers.length()) {
            return;
        }

        m_sortColumn = column;
        m_sortOrder = order;

        sort(column, order);

        emit sortChanged();
    }

    Q_INVOKABLE void applyFilters() {
        int colCount = m_headers.length();

        QString genericFilterStatement = "";
        if (!m_genericFilterString.isEmpty() && !m_genericFilterString.isNull()) {
            for (int i = 0; i < colCount; i++) {
                QString colName = m_headers.at(i);
                genericFilterStatement += colName + " LIKE '%" + m_genericFilterString + "%' OR ";
            }
        }

        if (genericFilterStatement.endsWith("OR ")) {
            genericFilterStatement.remove(genericFilterStatement.length() - 4, 3);
        }

        QString columnFilterStatement = "";
        for (int i = 0; i < colCount; i++) {
            QString colFilterString = m_columnFilterStrings.at(i);
            if (colFilterString != "") {
                QString colName = m_headers.at(i);
                columnFilterStatement += colName + " LIKE '%" + colFilterString + "%' AND ";
            }
        }

        if (columnFilterStatement.endsWith("AND ")) {
            columnFilterStatement.remove(columnFilterStatement.length() - 5, 4);
        }

        QString filterStatement = "";
        if (genericFilterStatement != "" && columnFilterStatement != "") {
            filterStatement += "(" + genericFilterStatement + ")";
            filterStatement += " AND ";
            filterStatement += "(" + columnFilterStatement + ")";
        } else if (genericFilterStatement != "") {
            filterStatement = genericFilterStatement;
        } else if (columnFilterStatement != "") {
            filterStatement = columnFilterStatement;
        }

        qCDebug(dbLog) << "genericFilterString:" << m_genericFilterString;
        qCDebug(dbLog) << "columnFilterStrings:" << m_columnFilterStrings;
        qCDebug(dbLog) << "filterStatement:" << filterStatement;

        setFilter(filterStatement);
    }

private:
    QStringList m_headers;

    int m_sortColumn = 0;
    Qt::SortOrder m_sortOrder = Qt::AscendingOrder;

    QString m_genericFilterString;
    QStringList m_columnFilterStrings;

signals:
    void tableChanged();
    void sortChanged();
    void genericFilterStringChanged();
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
    //     qCDebug(dbLog) << section;
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

#ifndef PRIVATEKEYMODEL_H
#define PRIVATEKEYMODEL_H

#include <QGuiApplication>
#include <QAbstractTableModel>
#include <QClipboard>

#include "global.h"
#include "databaseworker.h"

class PrivateKeyModel : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit PrivateKeyModel(DatabaseWorker *worker, QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public slots:
    bool canAddKey(const QString &key) const;

    void addKey(const QString &keyName, const QString &key);
    bool removeKey(int row);

    void copyKey(int row);

private:
    DatabaseWorker *worker;
    QList<PrivateKey> keys;
};

#endif // PRIVATEKEYMODEL_H

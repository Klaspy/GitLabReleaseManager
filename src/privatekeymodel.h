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
    explicit PrivateKeyModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE bool canAddKey(const QString &key) const;
    Q_INVOKABLE int getPKeyId(const int index) const;

public slots:
    void addKey(const QString &keyName, const QString &key);
    bool removeKey(int row);

    void copyKey(int row);

private slots:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QList<PrivateKey> keys;
};

#endif // PRIVATEKEYMODEL_H

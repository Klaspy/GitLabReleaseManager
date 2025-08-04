#include "privatekeymodel.h"

PrivateKeyModel::PrivateKeyModel(DatabaseWorker *worker, QObject *parent)
    : QAbstractTableModel(parent)
    , worker {worker}
{
    keys = worker->getPrivateKeys();
}

QVariant PrivateKeyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0: return tr("key name");
        case 1: return tr("key");
        default: return "";
        }
    }
    else
    {
        return section;
    }
}

int PrivateKeyModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return keys.size();
}

int PrivateKeyModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 2;
}

QVariant PrivateKeyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    int col = index.column();

    if (col > 2 || row > keys.size()) return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
    {
        switch (col)
        {
        case 0: return keys.at(row).name;
        case 1: return keys.at(row).key;
        }
    }
    }
    return QVariant();
}

bool PrivateKeyModel::canAddKey(const QString &key) const
{
    return !worker->containsPrivateKey(key);
}

void PrivateKeyModel::addKey(const QString &keyName, const QString &key)
{
    PrivateKey pKey;
    pKey.key = key;
    pKey.name = keyName;
    if (worker->addPrivateKey(pKey))
    {
        beginInsertRows({}, keys.size(), keys.size());
        keys.append(pKey);
        endInsertRows();
    }
}

bool PrivateKeyModel::removeKey(int row)
{
    if (row < 0 || row > keys.size())
        return false;

    beginRemoveRows({}, row, row);

    qDebug() << worker->deletePrivateKey(keys.at(row).id);
    keys.removeAt(row);

    endRemoveRows();
    return true;
}

void PrivateKeyModel::copyKey(int row)
{
    if (row < 0 || row > keys.size()) return;

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(keys.at(row).key);
}

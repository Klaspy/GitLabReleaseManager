#ifndef RELEASELISTMODEL_H
#define RELEASELISTMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QQmlEngine>
#include <QTimer>

#include <global.h>
#include <requester.h>

class ReleaseListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        ReleaseDataRole = 0x101,
        ReleaseDateTimeRole,
        CreateDateTimeRole
    };

    explicit ReleaseListModel(const int projectId, const QString &privateKey, QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    void setPrivateKey(QString newKey);

public slots:
    void getReleases();

private slots:
    void onGetReleasesDone(int projectId, QList<ReleaseData> releases);

private:
    int m_projectId;
    QString privateKey;
    QList<ReleaseData> m_releases;

    QTimer requestTimer;
};


class ReleaseSFPModel : public QSortFilterProxyModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

public:
    explicit ReleaseSFPModel(ReleaseListModel *source, QObject *parent = nullptr);

    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
};

#endif // RELEASELISTMODEL_H

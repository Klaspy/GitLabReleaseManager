#include "releaselistmodel.h"

ReleaseListModel::ReleaseListModel(const int projectId, const QString &privateKey, QObject *parent)
    : QAbstractListModel(parent)
    , m_projectId {projectId}
    , privateKey {privateKey}
{
    connect(Requester::globalInstance(), &Requester::getReleasesDone, this, &ReleaseListModel::onGetReleasesDone);

    requestTimer.setInterval(30'000);
    requestTimer.callOnTimeout(this, &getReleases);
    requestTimer.start();

    getReleases();
}

int ReleaseListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_releases.size();
}

QVariant ReleaseListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() > m_releases.size())
        return QVariant();

    int row = index.row();
    switch (role)
    {
    case ReleaseDataRole:
    {
        return QVariant::fromValue(m_releases.at(row));
        break;
    }
    case ReleaseDateTimeRole:
    {
        return m_releases.at(row).releaseDT;
        break;
    }
    case CreateDateTimeRole:
    {
        return m_releases.at(row).createDT;
        break;
    }
    default: return QVariant();
    }
}

QHash<int, QByteArray> ReleaseListModel::roleNames() const
{
    return {
        {ReleaseDataRole, "release_"},
        {ReleaseDateTimeRole, "release_dt_"},
        {CreateDateTimeRole, "create_dt_"}
    };
}

void ReleaseListModel::setPrivateKey(QString newKey)
{
    if (privateKey == newKey)
        return;

    privateKey = newKey;
}

void ReleaseListModel::getReleases()
{
    Requester::globalInstance()->getReleases(m_projectId, privateKey);
}

void ReleaseListModel::onGetReleasesDone(int projectId, QList<ReleaseData> releases)
{
    if (m_projectId != projectId)
        return;

    if (m_releases.isEmpty())
    {
        beginInsertRows({}, 0, releases.size() - 1);
        m_releases = releases;
        endInsertRows();
    }
    else
    {
        for (int i = 0; i < releases.size(); ++i)
        {
            if (m_releases.contains(releases.at(i)))
            {
                releases.removeAt(i);
            }
            else
            {
                i++;
            }
        }

        if (releases.isEmpty())
            return;

        QList<QString> tags;
        foreach (const ReleaseData &release, m_releases) {
            tags.append(release.tag);
        }

        foreach (const ReleaseData release, releases) {
            if (tags.contains(release.tag))
            {
                int i = tags.indexOf(release.tag);
                const ReleaseData &old = m_releases.at(i);

                QList<int> changedRoles {ReleaseDataRole};
                if (old.createDT != release.createDT) changedRoles.append(CreateDateTimeRole);
                if (old.releaseDT != release.releaseDT) changedRoles.append(ReleaseDateTimeRole);

                m_releases[i] = release;
                emit dataChanged(index(i), index(i), changedRoles);
                releases.removeOne(release);
            }
        }

        if (releases.isEmpty())
            return;

        beginInsertRows({}, m_releases.size(), m_releases.size() + releases.size() - 1);
        m_releases.append(releases);
        endInsertRows();
    }
}

ReleaseSFPModel::ReleaseSFPModel(ReleaseListModel *source, QObject *parent)
    : QSortFilterProxyModel {parent}
{
    setSourceModel(source);
    setSortRole(ReleaseListModel::ReleaseDateTimeRole);
    sort(0, Qt::DescendingOrder);
}

bool ReleaseSFPModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    ReleaseListModel::Roles sortBy = ReleaseListModel::Roles(sortRole());
    auto source = qobject_cast<ReleaseListModel*>(sourceModel());
    ReleaseData left  = source->data(source_left,  ReleaseListModel::ReleaseDateTimeRole).value<ReleaseData>();
    ReleaseData right = source->data(source_right, ReleaseListModel::ReleaseDateTimeRole).value<ReleaseData>();

    switch (sortBy)
    {
    case ReleaseListModel::ReleaseDateTimeRole:
    {
        return left.releaseDT < right.releaseDT;
        break;
    }
    case ReleaseListModel::CreateDateTimeRole:
    {
        return left.createDT < right.createDT;
        break;
    }
    default: return QSortFilterProxyModel::lessThan(source_left, source_right);
    }
}

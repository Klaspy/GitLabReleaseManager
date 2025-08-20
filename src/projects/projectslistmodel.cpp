#include "projectslistmodel.h"

ProjectsListModel::ProjectsListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    auto requester = Requester::globalInstance();

    connect(requester, &Requester::getProjectDone, this, &ProjectsListModel::onGetProjectDone);
    connect(requester, &Requester::getProjectError, this, &ProjectsListModel::onGetProjectError);

    QList<ProjectData> pDataList = DatabaseWorker::globalInstance()->getProjects();

    foreach (const ProjectData pData, pDataList) {
        projects.append(new Project(pData, this));
    }
}

int ProjectsListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return projects.size();
}

QVariant ProjectsListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() > projects.size())
        return QVariant();

    int row = index.row();
    switch (role)
    {
    case ProjectRole:
    {
        return QVariant::fromValue(projects.at(row));
        break;
    }
    }

    return QVariant();
}

QHash<int, QByteArray> ProjectsListModel::roleNames() const
{
    return {
        {ProjectRole, "project_"}
    };
}

bool ProjectsListModel::containsProject(int projectId)
{
    foreach (const Project *project, projects) {
        if (project->id() == projectId)
        {
            return true;
        }
    }
    return false;
}

void ProjectsListModel::addProject(int projectId, PrivateKey privateKey)
{
    qApp->setOverrideCursor(QCursor(Qt::BusyCursor));
    foreach (const Project *project, projects) {
        if (project->id() == projectId)
        {
            return;
        }
    }
    tempProjectId = projectId;

    Requester::globalInstance()->getProject(projectId, privateKey.key);
}

Project *ProjectsListModel::project(int row)
{
    if (row > -1 && row < projects.size())
    {
        return projects.at(row);
    }
    return nullptr;
}

void ProjectsListModel::onGetProjectDone(ProjectData pData)
{
    if (tempProjectId && tempProjectId.value() == pData.id)
    {
        beginInsertRows({}, projects.size(), projects.size());
        projects.append(new Project(pData, this));
        endInsertRows();
        tempProjectId = std::nullopt;
        DatabaseWorker::globalInstance()->addProject(pData);
        emit projectAdded();
        qApp->restoreOverrideCursor();
    }
}

void ProjectsListModel::onGetProjectError(int projectId, Requester::RequestError error, QString note)
{
    if (tempProjectId && tempProjectId.value() == projectId)
    {
        tempProjectId = std::nullopt;
        switch (error)
        {
        case Requester::Ok: break;
        case Requester::NetworkError:
        {
            emit addProjectError(tr("Error connection with server: %1").arg(note));
            break;
        }
        case Requester::HttpCodeError:
        {
            emit addProjectError(tr("Project not found, the server returned http code:%1").arg("note"));
            break;
        }
        case Requester::NotFoundError:
        {
            emit addProjectError(tr("Project not found, check the correctness of the specified ID or use another key"));
            break;
        }
        case Requester::HostError:
        {
            emit addProjectError(tr("Cannot send request, invalid host"));
            break;
        }
        }
        qApp->restoreOverrideCursor();
    }
}

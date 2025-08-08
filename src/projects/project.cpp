#include "project.h"

Project::Project(const ProjectData &pData, bool fromDb, QObject *parent)
    : QObject {parent}
    , data {pData}
{
    Requester *requester = Requester::globalInstance();
    connect(requester, &Requester::getProjectDone, this, &Project::onGetProjectDone);
    connect(requester, &Requester::getProjectError, this, &Project::onGetProjectError);

    updateInfoTimer.setInterval(30'000);
    updateInfoTimer.callOnTimeout(this, &Project::requestProjectData);

    if (!fromDb)
    {
        requestProjectData();
    }
}

void Project::setPrivateKey(const PrivateKey &newPrivateKey)
{
    if (newPrivateKey.id < 0 || newPrivateKey == data.privateKey)
        return;

    data.privateKey = newPrivateKey;
    requestProjectData();
    emit privateKeyChanged();
}

void Project::setLocalRepo(const QString &newLocalRepo)
{
    if (data.pathToLocalRepo == newLocalRepo)
        return;
    data.pathToLocalRepo = newLocalRepo;
    DatabaseWorker::globalInstance()->updateProject(data);
    emit localRepoChanged();
}

void Project::onGetProjectDone(ProjectData pData)
{
    if (pData.id != data.id)
        return;

    ProjectData temp = data;
    pData.pathToLocalRepo = data.pathToLocalRepo;
    data = pData;
    if (data.webUrl   != temp.webUrl)   emit urlChanged();
    if (data.createDT != temp.createDT) emit createDTChanged();
    if (data.author   != temp.author)   emit authorChanged();

    DatabaseWorker::globalInstance()->updateProject(data);
}

void Project::onGetProjectError(int projectId, Requester::RequestError error, QString note)
{
    if (projectId != data.id)
        return;

    switch (error)
    {
    case Requester::Ok: break;
    case Requester::NetworkError:
    {
        setError(Warning, tr("Error connection with server: %1").arg(note));
        break;
    }
    case Requester::HttpCodeError:
    {
        setError(Warning, tr("Project not found, the server returned http code:%1").arg("note"));
        break;
    }
    case Requester::NotFoundError:
    {
        setError(Critical, tr("Project not found, check the correctness of the specified ID or use another key"));
        break;
    }
    case Requester::HostError:
    {
        setError(Warning, tr("Cannot send request, invalid host"));
        break;
    }
    }
}

void Project::requestProjectData()
{
    if (data.privateKey.id < 0)
        return;
    Requester::globalInstance()->getProject(data.id, data.privateKey.key);
}

void Project::setError(ProjectError newError, const QString &newErrorString)
{
    if (m_error == newError && m_errorString == newErrorString)
        return;
    m_error = newError;
    m_errorString = newErrorString;
    emit errorChanged();
}

#include "project.h"

Project::Project(const ProjectData &pData, QObject *parent)
    : QObject {parent}
    , data {pData}
{
    if (data.privateKeyId)
    {
        privateKey = DatabaseWorker::globalInstance()->getPrivateKey(data.privateKeyId.value());
    }
    requestProjectData();
}

void Project::setPrivateKeyId(const QVariant &newPrivateKeyId)
{
    if (!newPrivateKeyId.isValid())
        return;

    int newPKeyId = newPrivateKeyId.toInt();
    if (newPKeyId == data.privateKeyId)
        return;

    data.privateKeyId = newPKeyId;
    if (data.privateKeyId)
    {
        privateKey = DatabaseWorker::globalInstance()->getPrivateKey(data.privateKeyId.value());
        requestProjectData();
    }
    emit privateKeyIdChanged();
}

void Project::onGetProjectDone(ProjectData pData)
{
    if (pData.id != data.id)
        return;

    data.name = pData.name;
    data.webUrl = pData.webUrl;
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
    }
}

void Project::requestProjectData()
{
    if (privateKey.isEmpty())
        return;
    Requester::globalInstance()->getProject(data.id, privateKey);
}

void Project::setError(ProjectError newError, const QString &newErrorString)
{
    if (m_error == newError && m_errorString == newErrorString)
        return;
    m_error = newError;
    m_errorString = newErrorString;
    emit errorChanged();
}

#include "project.h"

Project::Project(const ProjectData &pData, QObject *parent)
    : QObject {parent}
    , data {pData}
{
    Requester *requester = Requester::globalInstance();
    connect(requester, &Requester::getProjectDone,  this, &Project::onGetProjectDone);
    connect(requester, &Requester::getProjectError, this, &Project::onGetProjectError);
    connect(requester, &Requester::getTagsDone,     this, &Project::onGetTagsDone);

    updateInfoTimer.setInterval(30'000);
    updateInfoTimer.callOnTimeout(this, &Project::requestProjectData);
    updateInfoTimer.callOnTimeout(this, &Project::updateTags);
    updateInfoTimer.start();

    requestProjectData();
    updateTags();
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
    if (data.webUrl      != temp.webUrl)      emit urlChanged();
    if (data.createDT    != temp.createDT)    emit createDTChanged();
    if (data.author      != temp.author)      emit authorChanged();
    if (data.accessLevel != temp.accessLevel) emit accessLevelChanged();

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

bool sortTagsFromLast(const TagData &td1, const TagData &td2)
{
    QString v1 = td1.name, v2 = td2.name;
    QRegularExpression exp {"[^0-9\\.]"};
    v1.replace('-', '.').remove(exp);
    v2.replace('-', '.').remove(exp);

    QList<QString> vl1 = v1.split('.'), vl2 = v2.split('.');
    while (vl1.size() < 4) vl1.append("0");
    while (vl2.size() < 4) vl2.append("0");

    for (int i = 0; i < 4; ++i)
    {
        if (vl1.at(i).toInt() != vl2.at(i).toInt())
        {
            return vl1.at(i) > vl2.at(i);
        }
    }
    return false;
}

void Project::onGetTagsDone(int projectId, QList<TagData> tags)
{
    std::sort(tags.begin(), tags.end(), sortTagsFromLast);
    if (data.id != projectId || tags == m_tags)
        return;

    m_tags = tags;
    emit tagsChanged();
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

QVariant Project::releases() const
{
    return QVariant::fromValue(/*sortedReleases*/m_releases);
}

int Project::accessLevel() const
{
    return data.accessLevel;
}

void Project::updateTags()
{
    Requester::globalInstance()->getTags(data.id, data.privateKey.key);
}

QList<TagData> Project::tags() const
{
    return m_tags;
}

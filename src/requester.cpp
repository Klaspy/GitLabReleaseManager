#include "requester.h"
#include "databaseworker.h"

const QRegularExpression Requester::projectExp  {"^projects\\/\\d+$"};
const QRegularExpression Requester::releasesExp {"^projects\\/\\d+\\/releases$"};
const QRegularExpression Requester::tagsExp     {"^projects\\/\\d+\\/repository/tags$"};

Requester::Requester(QObject *parent)
    : QNetworkAccessManager {parent}
{
    setTransferTimeout(10'000);
    setAutoDeleteReplies(true);
    connect(this, &Requester::finished, this, &Requester::onReplyFinished);
}

Requester *Requester::globalInstance()
{
    static Requester *requester = new Requester(qApp);
    return requester;
}

void Requester::onReplyFinished(QNetworkReply *reply)
{
    QVariant statusCodeAttr = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    QNetworkRequest request = reply->request();
    QString path = request.url().path();
    path = path.split("api/v4/").last();

    if (!statusCodeAttr.isValid())
    {
        QString error = reply->errorString();
        if (projectExp.match(path).hasMatch())
        {
            int projectId = path.split("/").at(1).toInt();
            if (reply->error() == QNetworkReply::ProtocolUnknownError)
            {
                emit getProjectError(projectId, HostError, error);
            }
            else
            {
                emit getProjectError(projectId, NetworkError, error);
            }
        }

        qWarning() << "Ошибка во время выполнения запроса" << request.url().toString() << ":" << error;
        return;
    }

    QByteArray data = reply->readAll();
    QHttpHeaders headers = request.headers();
    int statusCode = statusCodeAttr.toInt();
    switch (statusCode)
    {
    case 200:
    {
        if (projectExp.match(path).hasMatch())
        {
            QJsonObject projectObj = QJsonDocument::fromJson(data).object();

            ProjectData pData;
            pData.id       = projectObj.value("id").toInt();
            pData.name     = projectObj.value("name").toString();
            pData.webUrl   = projectObj.value("web_url").toString();
            pData.createDT = QDateTime::fromString(projectObj.value("created_at").toString(), Qt::ISODate);
            pData.accessLevel = projectObj.value("permissions").toObject().value("project_access").
                                toObject().value("access_level").toInt();

            QJsonObject owner = projectObj.value("owner").toObject();
            pData.author.gitId  = owner.value("id").toInt();
            pData.author.name   = owner.value("name").toString();
            pData.author.gitUrl = owner.value("web_url").toString();

            pData.privateKey = DatabaseWorker::globalInstance()->getPrivateKey(request.rawHeader("PRIVATE-TOKEN"));

            emit getProjectDone(pData);
        }
        else if (releasesExp.match(path).hasMatch())
        {
            QList<ReleaseData> releases;
            QJsonArray releasesArr = QJsonDocument::fromJson(data).array();
            for (int i = 0, total = releasesArr.size(); i < total; ++i)
            {
                QJsonObject releaseObj = releasesArr.at(i).toObject();
                ReleaseData release;
                release.name        = releaseObj.value("name").toString();
                release.description = releaseObj.value("description").toString();
                release.tag         = releaseObj.value("tag_name").toString();
                release.commit      = QByteArray::fromHex(releaseObj.value("commit").toObject().
                                                     value("id").toString().toUtf8());
                release.url         = releaseObj.value("_links").toObject().value("self").toString();
                release.createDT    = QDateTime::fromString(releaseObj.value("created_at").toString(),
                                                            Qt::ISODate);
                release.releaseDT   = QDateTime::fromString(releaseObj.value("released_at").toString(),
                                                            Qt::ISODate);

                QJsonObject author = releaseObj.value("author").toObject();
                release.author.gitId  = author.value("id").toInt();
                release.author.name   = author.value("name").toString();
                release.author.gitUrl = author.value("web_url").toString();

                QJsonArray sources = releaseObj.value("assets").toObject().value("sources").toArray();
                for (int j = 0; j < sources.size(); j++)
                {
                    QJsonObject source = sources.at(j).toObject();
                    ReleaseLink link;
                    link.name = source.value("format").toString();
                    link.url  = source.value("url").toString();

                    release.sourceCodes.append(link);
                }

                QJsonArray linkArr = releaseObj.value("assets").toObject().value("links").toArray();
                for (int j = 0; j < linkArr.size(); j++)
                {
                    QJsonObject linkObj = linkArr.at(j).toObject();
                    ReleaseLink link;
                    link.name = linkObj.value("name").toString();
                    link.url  = linkObj.value("url").toString();
                    QString type = linkObj.value("link_type").toString();

                    if      (type == "package") release.packages.append(link);
                    else if (type == "image")   release.images.append(link);
                    else if (type == "runbook") release.runbooks.append(link);
                    else                        release.otherLinks.append(link);
                }

                releases.append(release);
            }

            int projectId = path.split("/").at(1).toInt();
            emit getReleasesDone(projectId, releases);
        }
        else if (tagsExp.match(path).hasMatch())
        {
            QJsonArray tagsArr = QJsonDocument::fromJson(data).array();

            QList<TagData> tags;
            for (const QJsonValue &tagVal : tagsArr)
            {
                QJsonObject tagObj = tagVal.toObject();
                if (tagObj.isEmpty())
                    continue;

                TagData tag;
                tag.name = tagObj.value("name").toString();
                tag.hasRelease = !tagObj.value("release").toObject().isEmpty();

                tags.append(tag);
            }

            int projectId = path.split("/").at(1).toInt();
            emit getTagsDone(projectId, tags);
        }
        break;
    }
    case 404:
    {
        if (projectExp.match(path).hasMatch())
        {
            int projectId = path.split("/").at(1).toInt();
            emit getProjectError(projectId, NotFoundError);
        }
        break;
    }
    default:
    {
        if (projectExp.match(path).hasMatch())
        {
            int projectId = path.split("/").at(1).toInt();
            emit getProjectError(projectId, HttpCodeError, QString::number(statusCode));
        }
        break;
    }
    }
}

void Requester::getProject(const int id, const QString privateKey)
{
    QNetworkRequest request = createNetworkRequest("projects/" + QString::number(id), privateKey);

    if (!request.url().isValid())
    {
        emit getProjectError(id, RequestError::HostError);
        return;
    }

    get(request);
}

void Requester::getReleases(const int id, const QString privateKey)
{
    QNetworkRequest request = createNetworkRequest(QString("projects/%1/releases").arg(id), privateKey);

    if (!request.url().isValid())
    {
        return;
    }

    get(request);
}

void Requester::getTags(const int id, const QString privateKey)
{
    QNetworkRequest request = createNetworkRequest(QString("projects/%1/repository/tags").arg(id), privateKey);

    if (!request.url().isValid())
    {
        return;
    }

    get(request);
}

QString Requester::getBaseUrl() const
{
    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/settings.ini",
                       QSettings::IniFormat);

    return settings.value("gitLabUrl").toString() + "/api/v4/";
}

QNetworkRequest Requester::createNetworkRequest(QString urlEndPath, QString privateKey, QUrlQuery query)
{
    QUrl url(getBaseUrl() + urlEndPath);
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setRawHeader("PRIVATE-TOKEN", privateKey.toUtf8());
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");

    return request;
}

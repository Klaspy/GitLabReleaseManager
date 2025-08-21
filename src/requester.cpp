#include "requester.h"
#include "databaseworker.h"

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

void Requester::getProject(const int id, const QString privateKey)
{
    QNetworkRequest request = createNetworkRequest("projects/" + QString::number(id), privateKey);

    if (!request.url().isValid())
    {
        emit getProjectError(id, RequestError::HostError);
        return;
    }

    QNetworkReply *reply = get(request);
    reply->setProperty("method", "get");
    reply->setProperty("id", id);
    reply->setProperty("requestType", GetProjectType);

}

void Requester::getReleases(const int id, const QString privateKey)
{
    QNetworkRequest request = createNetworkRequest(QString("projects/%1/releases").arg(id), privateKey);

    if (!request.url().isValid())
    {
        return;
    }

    QNetworkReply *reply =  get(request);
    reply->setProperty("method", "get");
    reply->setProperty("id", id);
    reply->setProperty("requestType", GetReleasesType);
}

void Requester::getTags(const int id, const QString privateKey)
{
    QNetworkRequest request = createNetworkRequest(QString("projects/%1/repository/tags").arg(id), privateKey);

    if (!request.url().isValid())
    {
        return;
    }

    QNetworkReply *reply = get(request);
    reply->setProperty("method", "get");
    reply->setProperty("id", id);
    reply->setProperty("requestType", GetTagsType);
}

QList<ReleaseLink> Requester::uploadFiles(const int id, const QString privateKey, const QString &package,
                                          const QString &version, const QList<ReleaseLink> &files)
{
    if (files.isEmpty())
        return {};

    QList<ReleaseLink> result;
    QNetworkReply *reply;
    QNetworkRequest request;
    QEventLoop loop;
    int statusCode;
    int packageId {-1};
    QSettings settings(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/settings.ini",
                       QSettings::IniFormat);

    QString gitUrl = settings.value("gitLabUrl").toString();
    for (const ReleaseLink &releaseFile : files)
    {
        request = createNetworkRequest(QString("projects/%1/packages/generic/%2/%3/%4")
                                                           .arg(id)
                                                           .arg(package, version, releaseFile.name)
                                                           , privateKey);

        QFile file(QUrl(releaseFile.url).toString(QUrl::PreferLocalFile));

        QUrl url = QUrl(releaseFile.url);
        if (!url.isLocalFile() && url.isValid())
        {
            result.append(releaseFile);
            continue;
        }
        if (!request.url().isValid() || !url.isLocalFile() || !file.open(QIODevice::ReadOnly))
        {
            result.append(releaseFile);
            continue;
        }

        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
        request.setHeader(QNetworkRequest::ContentLengthHeader, file.size());

        reply = put(request, file.readAll());
        reply->setProperty("requestType", UndefinedType);

        connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        loop.exec();

        statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (reply->error() != QNetworkReply::NoError && (statusCode != 200 || statusCode != 201))
        {
            result.append(releaseFile);
            continue;
        }

        result.append(releaseFile);
    }

    packageId = getProjectPackage(id, privateKey, package, version);
    if (packageId == -1)
        return {};

    request = createNetworkRequest(QString("projects/%1/packages/%2/package_files").arg(id).arg(packageId),
                                   privateKey);
    reply = get(request);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QJsonArray packageFiles = QJsonDocument::fromJson(reply->readAll()).array();

    int total = result.size();
    for (const QJsonValue &packageFileVal : std::as_const(packageFiles))
    {
        QJsonObject packageFileObj = packageFileVal.toObject();
        QString name = packageFileObj.value("file_name").toString();
        qint64 id = packageFileObj.value("id").toInteger(-1);
        if (id == -1 || name.isEmpty())
            continue;

        for (int i = 0; i < total; ++i)
        {
            if (result.at(i).name == name)
            {
                result[i].url = QString("%2/-/package_files/%1/download").arg(id);
            }
        }
    }

    return result;
}

bool Requester::createRelease(const int id, const QString privateKey, const QString &tag, const QString &title,
                              const QString &description, const QList<ReleaseLink> &links)
{

    QNetworkRequest request = createNetworkRequest(QString("projects/%1/releases").arg(id), privateKey);

    QJsonObject releaseData {
        {"name", title},
        {"tag_name", tag},
        {"description", description}
    };

    QJsonArray linksArr;

    for (const ReleaseLink &link : links)
    {
        QJsonObject linkObj {
            {"name", link.name},
            {"url", link.url},
            {"link_type", linkTypetoString(link.type)},
            {"filepath", "/" + link.name}
        };

        linksArr.append(linkObj);
    }

    releaseData.insert("assets", QJsonObject {
                                             {"links", linksArr}
    });

    QEventLoop loop;
    QNetworkReply *reply = post(request, QJsonDocument(releaseData).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (reply->error() != QNetworkReply::NoError && (statusCode != 200 || statusCode != 201))
    {
        return false;
    }
    return true;
}

void Requester::onReplyFinished(QNetworkReply *reply)
{
    RequestType requestType = (RequestType)reply->property("requestType").toInt();
    if (requestType == UndefinedType)
        return;
    int projectId = reply->property("id").toInt();
    QString method = reply->property("method").toString();

    QVariant statusCodeAttr = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    QNetworkRequest request = reply->request();
    QString path = request.url().path();
    path = path.split("api/v4/").last();

    if (!statusCodeAttr.isValid())
    {
        QString error = reply->errorString();
        switch (requestType)
        {
        case GetProjectType:
        {
            if (reply->error() == QNetworkReply::ProtocolUnknownError)
            {
                emit getProjectError(projectId, HostError, error);
            }
            else
            {
                emit getProjectError(projectId, NetworkError, error);
            }
            break;
        }
        default: break;
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
        switch (requestType)
        {
        case UndefinedType: Q_UNREACHABLE(); break;
        case GetProjectType:
        {
            QJsonObject projectObj = QJsonDocument::fromJson(data).object();

            ProjectData pData;
            pData.id       = projectObj.value("id").toInt();
            pData.name     = projectObj.value("name").toString();
            pData.webUrl   = projectObj.value("web_url").toString();
            pData.createDT = QDateTime::fromString(projectObj.value("created_at").toString(), Qt::ISODate);
            pData.accessLevel = projectObj.value("permissions").toObject().value("project_access").
                                toObject().value("access_level").toInt();

            // TODO Обработать отсутствие поля owner
            QJsonObject owner = projectObj.value("owner").toObject();
            pData.author.gitId  = owner.value("id").toInt();
            pData.author.name   = owner.value("name").toString();
            pData.author.gitUrl = owner.value("web_url").toString();

            pData.privateKey = DatabaseWorker::globalInstance()->getPrivateKey(request.rawHeader("PRIVATE-TOKEN"));

            emit getProjectDone(pData);
            break;
        }
        case GetReleasesType:
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

            emit getReleasesDone(projectId, releases);
            break;
        }
        case GetTagsType:
        {
            QJsonArray tagsArr = QJsonDocument::fromJson(data).array();

            QList<TagData> tags;
            for (const QJsonValue &tagVal : std::as_const(tagsArr))
            {
                QJsonObject tagObj = tagVal.toObject();
                if (tagObj.isEmpty())
                    continue;

                TagData tag;
                tag.name = tagObj.value("name").toString();
                tag.hasRelease = !tagObj.value("release").toObject().isEmpty();

                tags.append(tag);
            }

            emit getTagsDone(projectId, tags);
            break;
        }
        }
        break;
    }
    case 404:
    {
        if (requestType == GetProjectType)
        {
            emit getProjectError(projectId, NotFoundError);
        }
        break;
    }
    default:
    {
        switch (requestType)
        {
        case GetProjectType:
        {
            emit getProjectError(projectId, HttpCodeError, QString::number(statusCode));
            break;
        }
        default: break;
        }
        break;
    }
    }
}

qint64 Requester::getProjectPackage(const int id, const QString privateKey, const QString &package, const QString &version)
{
    QEventLoop loop;
    QNetworkRequest request = createNetworkRequest(QString("projects/%1/packages").arg(id), privateKey);
    QNetworkReply *reply = get(request);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (reply->error() != QNetworkReply::NoError && (statusCode != 200 || statusCode != 201))
    {
        return {};
    }

    QJsonArray packages = QJsonDocument::fromJson(reply->readAll()).array();

    for (const QJsonValue &packageVal : std::as_const(packages))
    {
        QJsonObject packageObj = packageVal.toObject();
        if (packageObj.value("name").toString() == package &&
            packageObj.value("version").toString() == version)
        {
            return packageObj.value("id").toInteger();
        }
    }
    return -1;
}

QString Requester::linkTypetoString(ReleaseLink::LinkType type)
{
    switch (type)
    {
    case ReleaseLink::SourceCode: return "other";
    case ReleaseLink::Package: return "package";
    case ReleaseLink::Image: return "image";
    case ReleaseLink::RunBook: return "runbook";
    case ReleaseLink::Other: return "other";
    }

    Q_UNREACHABLE();
    return {};
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

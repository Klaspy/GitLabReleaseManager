#include "requester.h"
#include "databaseworker.h"

const QRegularExpression Requester::projectExp {"^projects\\/\\d+$"};

Requester::Requester(QObject *parent)
    : QNetworkAccessManager {parent}
{
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
    }

    QByteArray data = reply->readAll();
    qDebug() << request.url();
    QHttpHeaders headers = request.headers();
    for (int i = 0; i < headers.size(); ++i)
    {
        qDebug() << headers.nameAt(i) << headers.valueAt(i);
    }
    qDebug() << request.rawHeaderList().join('\n');
    qDebug() << data;
    int statusCode = statusCodeAttr.toInt();
    switch (statusCode)
    {
    case 200:
    {
        if (projectExp.match(path).hasMatch())
        {
            QJsonObject projectObj = QJsonDocument::fromJson(data).object();

            ProjectData pData;
            pData.id     = projectObj.value("id").toInt();
            pData.name   = projectObj.value("name").toString();
            pData.webUrl = projectObj.value("web_url").toString();

            pData.privateKeyId = DatabaseWorker::globalInstance()->getPrivateKeyId(request.rawHeader("PRIVATE-TOKEN"));

            emit getProjectDone(pData);
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
    qDebug() << privateKey.toUtf8();
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
    qDebug() << request.rawHeader("PRIVATE-TOKEN");

    return request;
}

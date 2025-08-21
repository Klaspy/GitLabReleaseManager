#ifndef REQUESTER_H
#define REQUESTER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QObject>
#include <QSettings>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

#include "global.h"

class Requester : public QNetworkAccessManager
{
    Q_OBJECT

    explicit Requester(QObject *parent = nullptr);

    enum RequestType {
        UndefinedType = 0,
        GetProjectType,
        GetReleasesType,
        GetTagsType
    };

public:
    enum RequestError {
        Ok,
        NetworkError,
        HttpCodeError,
        NotFoundError,
        HostError
    };

    static Requester *globalInstance();

    void getProject(const int id, const QString privateKey);
    void getReleases(const int id, const QString privateKey);
    void getTags(const int id, const QString privateKey);
    QList<ReleaseLink> uploadFiles(const int id, const QString privateKey, const QString &package, const QString &version,
                     const QList<ReleaseLink> &files);

    bool createRelease(const int id, const QString privateKey, const QString &tag, const QString &title,
                       const QString &description, const QList<ReleaseLink> &links);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    qint64 getProjectPackage(const int id, const QString privateKey, const QString &package, const QString &version);

    static QString linkTypetoString(ReleaseLink::LinkType type);

signals:
    void getProjectDone(ProjectData pData);
    void getProjectError(int projectId, RequestError error, QString note = "");

    void getReleasesDone(int projectId, QList<ReleaseData> releases);

    void getTagsDone(int projectId, QList<TagData> tags);

private:
    QString getBaseUrl() const;

    QNetworkRequest createNetworkRequest(QString urlEndPath, QString privateKey, QUrlQuery query = QUrlQuery());
};

#endif // REQUESTER_H

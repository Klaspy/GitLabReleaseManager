#ifndef REQUESTER_H
#define REQUESTER_H

#include <QNetworkAccessManager>
#include <QObject>
#include <QSettings>
#include <QStandardPaths>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "global.h"

class Requester : public QNetworkAccessManager
{
    Q_OBJECT

    explicit Requester(QObject *parent = nullptr);
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

private slots:
    void onReplyFinished(QNetworkReply *reply);

signals:
    void getProjectDone(ProjectData pData);
    void getProjectError(int projectId, RequestError error, QString note = "");

    void getReleasesDone(int projectId, QList<ReleaseData> releases);

    void getTagsDone(int projectId, QList<TagData> tags);

private:
    QString getBaseUrl() const;

    QNetworkRequest createNetworkRequest(QString urlEndPath, QString privateKey, QUrlQuery query = QUrlQuery());

    static const QRegularExpression projectExp;
    static const QRegularExpression releasesExp;
    static const QRegularExpression tagsExp;
};

#endif // REQUESTER_H

#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>
#include <QTimer>

#include "src/global.h"
#include "src/databaseworker.h"
#include "src/requester.h"

class Project : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(int        id         READ id CONSTANT                                             FINAL)
    Q_PROPERTY(PrivateKey privateKey READ privateKey WRITE setPrivateKey NOTIFY privateKeyChanged FINAL)
    Q_PROPERTY(QString    name       READ name                           NOTIFY nameChanged       FINAL)
    Q_PROPERTY(QString    url        READ url                            NOTIFY urlChanged        FINAL)
    Q_PROPERTY(QString    localRepo  READ localRepo  WRITE setLocalRepo  NOTIFY localRepoChanged  FINAL)
    Q_PROPERTY(QDateTime  createDT   READ createDT                       NOTIFY createDTChanged   FINAL)
    Q_PROPERTY(UserData   author     READ author                         NOTIFY authorChanged     FINAL)

    Q_PROPERTY(ProjectError error  READ error       NOTIFY errorChanged FINAL)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorChanged FINAL)

public:
    enum ProjectError {
        Ok,
        Warning,
        Critical
    };
    Q_ENUM(ProjectError)

    explicit Project(const ProjectData &pData, bool fromDb, QObject *parent = nullptr);

    int id() const {return data.id;}

    QString name() const {return data.name;}

    PrivateKey privateKey() const {return data.privateKey;}
    void setPrivateKey(const PrivateKey &newPrivateKey);

    QString url() const {return data.webUrl;}

    QString localRepo() const {return data.pathToLocalRepo;}
    void setLocalRepo(const QString &newLocalRepo);

    QDateTime createDT() const {return data.createDT;}

    UserData author() const {return data.author;}

    ProjectError error() const {return m_error;}
    QString errorString() const {return m_errorString;}

signals:
    void nameChanged();
    void privateKeyChanged();
    void urlChanged();
    void localRepoChanged();
    void createDTChanged();
    void authorChanged();

    void errorChanged();
    void errorStringChanged();

private slots:
    void onGetProjectDone(ProjectData pData);
    void onGetProjectError(int projectId, Requester::RequestError error, QString note);

    void requestProjectData();

private:
    ProjectData data;
    ProjectError m_error {Ok};
    QString m_errorString;

    QTimer updateInfoTimer;

    void setError(ProjectError newError, const QString &newErrorString = "");
};
// Q_DECLARE_METATYPE(Project)

#endif // PROJECT_H

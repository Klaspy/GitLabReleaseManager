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

    Q_PROPERTY(int      id           READ id CONSTANT                                                   FINAL)
    Q_PROPERTY(QString  name         READ name                               NOTIFY nameChanged         FINAL)
    Q_PROPERTY(QVariant privateKeyId READ privateKeyId WRITE setPrivateKeyId NOTIFY privateKeyIdChanged FINAL)

    Q_PROPERTY(ProjectError error  READ error       NOTIFY errorChanged FINAL)
    Q_PROPERTY(QString errorString READ errorString NOTIFY errorChanged FINAL)

public:
    enum ProjectError {
        Ok,
        Warning,
        Critical
    };
    Q_ENUM(ProjectError)

    explicit Project(const ProjectData &pData, QObject *parent = nullptr);

    int id() const {return data.id;}

    QString name() const {return data.name;}

    QVariant privateKeyId() const {return data.privateKeyId ? data.privateKeyId.value() : QVariant();}
    void setPrivateKeyId(const QVariant &newPrivateKeyId);

    ProjectError error() const {return m_error;}
    QString errorString() const {return m_errorString;}

signals:
    void nameChanged();
    void privateKeyIdChanged();

    void errorChanged();
    void errorStringChanged();

private slots:
    void onGetProjectDone(ProjectData pData);
    void onGetProjectError(int projectId, Requester::RequestError error, QString note);

private:
    ProjectData data;
    ProjectError m_error {Ok};
    QString m_errorString;
    QString privateKey;

    void requestProjectData();

    void setError(ProjectError newError, const QString &newErrorString = "");
};
// Q_DECLARE_METATYPE(Project)

#endif // PROJECT_H

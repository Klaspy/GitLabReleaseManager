#ifndef DATABASEWORKER_H
#define DATABASEWORKER_H

#include <QCoreApplication>
#include <QQmlEngine>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QStandardPaths>

#include "global.h"

class DatabaseWorker : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    explicit DatabaseWorker(QObject *parent = nullptr);
public:
    static DatabaseWorker *globalInstance();

    QList<ProjectData> getProjects() const;
    bool addProject(const ProjectData &project);
    bool updateProject(const ProjectData &project);
    bool deleteProject(const int &projectId);
    bool containsProject(const int &projectId);

    QList<PrivateKey> getPrivateKeys() const;
    bool addPrivateKey(PrivateKey &privateKey);
    bool deletePrivateKey(const int &keyId);
    bool containsPrivateKey(const QString &key);
    bool containsPrivateKey(const int &keyId);
    QString getPrivateKey(const int &keyId);
    int getPrivateKeyId(const QString &key);

signals:
    void privateKeysChanged();

private:
    QSqlDatabase db;

    void init();
};

#endif // DATABASEWORKER_H

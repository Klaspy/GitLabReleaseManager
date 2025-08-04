#ifndef DATABASEWORKER_H
#define DATABASEWORKER_H

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
public:
    explicit DatabaseWorker(QObject *parent = nullptr);

    QList<Project> getProjects() const;
    bool addProject(const Project &project);
    bool updateProject(const Project &project);
    bool deleteProject(const int &projectId);
    bool containsProject(const int &projectId);

    QList<PrivateKey> getPrivateKeys() const;
    bool addPrivateKey(PrivateKey &privateKey);
    bool deletePrivateKey(const int &keyId);
    bool containsPrivateKey(const QString &key);
    bool containsPrivateKey(const int &keyId);

signals:
    void privateKeysChanged();

private:
    QSqlDatabase db;

    void init();
};

#endif // DATABASEWORKER_H

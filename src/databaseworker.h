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
    bool containsProject(const int &projectId) const;

    QList<PrivateKey> getPrivateKeys() const;
    bool addPrivateKey(PrivateKey &privateKey);
    bool deletePrivateKey(const int &keyId);
    bool containsPrivateKey(const QString &key) const;
    bool containsPrivateKey(const int &keyId) const;
    PrivateKey getPrivateKey(const int &keyId) const;
    PrivateKey getPrivateKey(const QString &keyStr) const;
    int getPrivateKeyId(const QString &key) const;

    QList<UserData> getUsers() const;
    UserData getUser(const int &userId) const;
    bool addUser(const UserData &data);
    bool updateUser(const UserData &data);
    bool containsUser(const int &userId) const;

signals:
    void privateKeysChanged();

private:
    QSqlDatabase db;

    void init();
};

#endif // DATABASEWORKER_H

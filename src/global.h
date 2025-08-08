#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QQmlEngine>

struct PrivateKey
{
    Q_GADGET
    QML_ELEMENT
public:
    int id {-1};
    QString name;
    QString key;

    bool operator ==(const PrivateKey &other) const
    {
        return id   == other.id   &&
               name == other.name &&
               key  == other.key;
    }
};

struct UserData
{
    Q_GADGET
    QML_ELEMENT
public:
    int gitId {-1};
    QString name;
    QString gitUrl;

    bool operator ==(const UserData &other) const
    {
        return gitId  == other.gitId &&
               name   == other.name  &&
               gitUrl == other.gitUrl;
    }

    bool operator !=(const UserData &other) const
    {
        return gitId  != other.gitId ||
               name   != other.name  ||
               gitUrl != other.gitUrl;
    }
};

struct ProjectData
{
    int id;
    PrivateKey privateKey;
    QString name;
    QString webUrl;
    QString pathToLocalRepo;
    QDateTime createDT;

    UserData author;
};

#endif // GLOBAL_H

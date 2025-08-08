#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QQmlEngine>

struct PrivateKey
{
    int id;
    QString name;
    QString key;
};

struct ProjectData
{
    int id;
    std::optional<int> privateKeyId {std::nullopt};
    QString name;
    QString webUrl;
    QString pathToLocalRepo;
};

#endif // GLOBAL_H

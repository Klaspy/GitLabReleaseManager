#ifndef GLOBAL_H
#define GLOBAL_H

#include <QString>
#include <QQmlEngine>

class GitLabAccessLevels
{
    Q_GADGET
    QML_ELEMENT
    // QML_NAMED_ELEMENT(Enums)

public:
    enum Enumerator {
        NoAccess = 0,
        Minimal = 5,
        Guest = 10,
        Reporter = 20,
        Developer = 30,
        Maintainer = 40,
        Owner = 50
    };
    Q_ENUM(Enumerator)
};

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
    QML_UNCREATABLE("")

    Q_PROPERTY(int     gitId  MEMBER gitId  FINAL)
    Q_PROPERTY(QString name   MEMBER name   FINAL)
    Q_PROPERTY(QString gitUrl MEMBER gitUrl FINAL)

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
Q_DECLARE_METATYPE(UserData)

struct ProjectData
{
    int id;
    PrivateKey privateKey;
    QString name;
    QString webUrl;
    QString pathToLocalRepo;
    QDateTime createDT;
    int accessLevel {GitLabAccessLevels::NoAccess};

    UserData author;
};

struct ReleaseLink
{
    Q_GADGET
    QML_ELEMENT

    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(QString url  MEMBER url)
    Q_PROPERTY(LinkType type  MEMBER type)

public:
    enum LinkType {
        SourceCode,
        Package,
        Image,
        RunBook,
        Other
    } type {Other};
    Q_ENUM(LinkType)

    QString name;
    QString url;

    Q_INVOKABLE explicit ReleaseLink() {}

    bool operator ==(const ReleaseLink &other) const
    {
        return name == other.name && url == other.url;
    }
};

struct ReleaseData
{
    Q_GADGET
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QString    name        MEMBER name)
    Q_PROPERTY(QString    description MEMBER description)
    Q_PROPERTY(UserData   author      MEMBER author)
    Q_PROPERTY(QString    tag         MEMBER tag)
    Q_PROPERTY(QByteArray commit      MEMBER commit)
    Q_PROPERTY(QDateTime  createDT    MEMBER createDT)
    Q_PROPERTY(QDateTime  releaseDT   MEMBER releaseDT)

    Q_PROPERTY(QList<ReleaseLink> sourceCodes MEMBER sourceCodes)
    Q_PROPERTY(QList<ReleaseLink> packages    MEMBER packages)
    Q_PROPERTY(QList<ReleaseLink> images      MEMBER images)
    Q_PROPERTY(QList<ReleaseLink> runbooks    MEMBER runbooks)
    Q_PROPERTY(QList<ReleaseLink> otherLinks  MEMBER otherLinks)


public:
    QString    name;
    QString    description;
    UserData   author;
    QString    tag;
    QByteArray commit;
    QString    url;
    QDateTime  createDT;
    QDateTime  releaseDT;
    QList<ReleaseLink> sourceCodes;
    QList<ReleaseLink> packages;
    QList<ReleaseLink> images;
    QList<ReleaseLink> runbooks;
    QList<ReleaseLink> otherLinks;

    Q_INVOKABLE QString releaseDTString() const {return releaseDT.toString("dd.MM.yyyy hh:mm:ss t");}

    bool operator ==(const ReleaseData &other) const
    {
        return name        == other.name &&
               description == other.description &&
               author      == other.author &&
               tag         == other.tag &&
               commit      == other.commit &&
               url         == other.url &&
               createDT    == other.createDT &&
               releaseDT   == other.releaseDT;
    }
};
Q_DECLARE_METATYPE(ReleaseData)

struct TagData
{
    Q_GADGET
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_PROPERTY(QString name       MEMBER name)
    Q_PROPERTY(bool    hasRelease MEMBER hasRelease)
    Q_PROPERTY(bool canCreateRelease READ canCreateRelease CONSTANT)

public:
    QString name;
    bool hasRelease;

    Q_INVOKABLE inline bool canCreateRelease() const {return !hasRelease;}

    bool operator ==(const TagData &other) const
    {
        return name == other.name && hasRelease == other.hasRelease;
    }
};
Q_DECLARE_METATYPE(TagData)

#endif // GLOBAL_H

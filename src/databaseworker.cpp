#include "databaseworker.h"

DatabaseWorker::DatabaseWorker(QObject *parent)
    : QObject{parent}
{
    db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    db.setDatabaseName(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/database.sqlite");
    qDebug() << db.databaseName();

    db.open();

    init();
}

DatabaseWorker *DatabaseWorker::globalInstance()
{
    static DatabaseWorker *worker = new DatabaseWorker(qApp);
    return worker;
}

QList<ProjectData> DatabaseWorker::getProjects() const
{
    QSqlQuery query("SELECT * FROM Projects ORDER BY Id");

    if (!query.exec())
    {
        qWarning() << "Не удалось получить список проектов" << query.lastError();
        return {};
    }

    QList<ProjectData> res;
    while (query.next())
    {
        QSqlRecord record = query.record();
        ProjectData project;
        project.id              = record.value("Id").toInt();
        if (!record.value("PrivateKey").isNull())
        {
            project.privateKey  = getPrivateKey(record.value("PrivateKey").toInt());
        }
        project.name            = record.value("ProjectName").toString();
        project.webUrl          = record.value("WebUrl").toString();
        project.pathToLocalRepo = record.value("LocalPath").toString();
        project.createDT        = QDateTime::fromSecsSinceEpoch(record.value("CreateTime").toLongLong(),
                                                         QTimeZone(0));
        project.author          = getUser(record.value("UserId").toInt());

        res.append(project);
    }

    return res;
}

bool DatabaseWorker::addProject(const ProjectData &project)
{
    if (containsProject(project.id)) return false;

    QSqlQuery query("INSERT INTO Projects (Id, ProjectName, WebUrl, LocalPath, PrivateKey, UserId, CreateTime ) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(project.id);
    query.addBindValue(project.name);
    query.addBindValue(project.webUrl);
    query.addBindValue(project.pathToLocalRepo.isEmpty() ? QVariant() : project.pathToLocalRepo);
    query.addBindValue(project.privateKey.id > -1 ? project.privateKey.id : QVariant());

    if (containsUser(project.author.gitId))
    {
        updateUser(project.author);
    }
    else
    {
        addUser(project.author);
    }
    query.addBindValue(project.author.gitId);
    query.addBindValue(project.createDT.toSecsSinceEpoch());

    if (!query.exec())
    {
        qWarning() << "Не удалось добавить новый проект в БД" << query.lastError();
        return false;
    }

    return true;
}

bool DatabaseWorker::updateProject(const ProjectData &project)
{
    if (!containsProject(project.id)) return false;

    QSqlQuery query("UPDATE Projects "
                    "SET ProjectName = ?, WebUrl = ?, LocalPath = ?, PrivateKey = ?, UserId = ?, CreateTime = ? "
                    "WHERE Id=?");

    query.addBindValue(project.name);
    query.addBindValue(project.webUrl);
    query.addBindValue(project.pathToLocalRepo.isEmpty() ? QVariant() : project.pathToLocalRepo);
    query.addBindValue(project.privateKey.id > -1 ? project.privateKey.id : QVariant());

    if (containsUser(project.author.gitId))
    {
        updateUser(project.author);
    }
    else
    {
        addUser(project.author);
    }
    query.addBindValue(project.author.gitId);
    query.addBindValue(project.createDT.toSecsSinceEpoch());
    query.addBindValue(project.id);

    if (!query.exec())
    {
        qWarning() << "Не удалось обновить проект в БД" << query.lastError();
        return false;
    }

    return true;
}

bool DatabaseWorker::deleteProject(const int &projectId)
{
    if (!containsProject(projectId)) return true;

    QSqlQuery query("DELETE FROM Projects WHERE Id=?");
    query.addBindValue(projectId);

    if (!query.exec())
    {
        qWarning() << "Не удалось удалить проект из БД" << query.lastError();
        return false;
    }
    return true;
}

bool DatabaseWorker::containsProject(const int &projectId) const
{
    QSqlQuery query("SELECT count(Id) FROM Projects WHERE Id=?");
    query.addBindValue(projectId);

    if (!query.exec())
    {
        qWarning() << "Ошибка проверки существования проекта с индексом" << projectId << query.lastError();
        return false;
    }

    query.next();
    return query.value(0).toBool();
}

QList<PrivateKey> DatabaseWorker::getPrivateKeys() const
{
    QSqlQuery query("SELECT * FROM PrivateKeys ORDER BY KeyName");

    if (!query.exec())
    {
        qWarning() << "Не удалось получить список приватных ключей" << query.lastError();
        return {};
    }

    QList<PrivateKey> res;
    while (query.next())
    {
        QSqlRecord record = query.record();
        PrivateKey key;
        key.name = record.value("KeyName").toString();
        key.key  = record.value("Key").toString();
        key.id   = record.value("Id").toInt();

        res.append(key);
    }

    return res;
}

bool DatabaseWorker::addPrivateKey(PrivateKey &privateKey)
{
    if (containsPrivateKey(privateKey.key)) return false;

    QSqlQuery query("INSERT INTO PrivateKeys (KeyName, Key) "
                    "VALUES (?, ?) "
                    "RETURNING (Id)");
    query.addBindValue(privateKey.name);
    query.addBindValue(privateKey.key);

    if (!query.exec())
    {
        qWarning() << "Не удалось добавить приватный ключ" << query.lastError();
        return false;
    }

    query.next();
    privateKey.id = query.value("Id").toInt();
    emit privateKeysChanged();
    return true;
}

bool DatabaseWorker::deletePrivateKey(const int &keyId)
{
    QSqlQuery query("DELETE FROM PrivateKeys WHERE Id=?");
    query.addBindValue(keyId);

    if (!query.exec())
    {
        qWarning() << "Не удалось удалить приватный ключ" << query.lastError();
        return false;
    }
    emit privateKeysChanged();
    return true;
}

bool DatabaseWorker::containsPrivateKey(const QString &key) const
{
    QSqlQuery query("SELECT count(Id) FROM PrivateKeys WHERE Key=?");
    query.addBindValue(key);

    if (!query.exec())
    {
        qWarning() << "Ошибка во время выполнения SELECT count(Id) FROM PrivateKeys WHERE Key=?."
                   << query.lastError();
        return false;
    }
    query.next();
    return query.value(0).toBool();
}

bool DatabaseWorker::containsPrivateKey(const int &keyId) const
{
    QSqlQuery query("SELECT count(Id) FROM PrivateKeys WHERE Id=?");
    query.addBindValue(keyId);

    if (!query.exec())
    {
        qWarning() << "Ошибка во время выполнения SELECT count(Id) FROM PrivateKeys WHERE Id=?."
                   << query.lastError();
        return false;
    }
    query.next();
    return query.value(0).toBool();
}

PrivateKey DatabaseWorker::getPrivateKey(const int &keyId) const
{
    QSqlQuery query("SELECT * FROM PrivateKeys WHERE Id=?");
    query.addBindValue(keyId);

    PrivateKey key;
    if (!query.exec())
    {
        qWarning() << "Ошибка во время выполнения SELECT Key FROM PrivateKeys WHERE Id=?."
                   << query.lastError();
        return key;
    }

    if (query.next())
    {
        QSqlRecord record {query.record()};
        key.id = record.value("Id").toInt();
        key.name = record.value("KeyName").toString();
        key.key = record.value("Key").toString();
    }

    return key;
}

PrivateKey DatabaseWorker::getPrivateKey(const QString &keyStr) const
{
    QSqlQuery query("SELECT * FROM PrivateKeys WHERE Key=?");
    query.addBindValue(keyStr);

    PrivateKey key;
    if (!query.exec())
    {
        qWarning() << "Ошибка во время выполнения SELECT Key FROM PrivateKeys WHERE Key=?."
                   << query.lastError();
        return key;
    }

    if (query.next())
    {
        QSqlRecord record {query.record()};
        key.id = record.value("Id").toInt();
        key.name = record.value("KeyName").toString();
        key.key = record.value("Key").toString();
    }

    return key;
}

int DatabaseWorker::getPrivateKeyId(const QString &key) const
{
    QSqlQuery query("SELECT Id FROM PrivateKeys WHERE key=?");
    query.addBindValue(key);

    if (!query.exec())
    {
        qWarning() << "Ошибка во время выполнения SELECT Id FROM PrivateKeys WHERE Id=?."
                   << query.lastError();
        return false;
    }

    if (query.next())
    {
        return query.value(0).toBool();
    }
    return -1;
}

QList<UserData> DatabaseWorker::getUsers() const
{
    QSqlQuery query("SELECT * FROM Users ORDER BY Id");

    if (!query.exec())
    {
        qWarning() << "Не удалось получить список пользователей" << query.lastError();
        return {};
    }

    QList<UserData> res;
    while (query.next())
    {
        QSqlRecord record = query.record();
        UserData user;
        user.gitId  = record.value("Id").toInt();
        user.name   = record.value("Name").toString();
        user.gitUrl = record.value("WebUrl").toString();

        res.append(user);
    }

    return res;
}

UserData DatabaseWorker::getUser(const int &userId) const
{
    QSqlQuery query("SELECT * FROM Users WHERE Id=?");
    query.addBindValue(userId);

    UserData user;
    if (!query.exec())
    {
        qWarning() << "Не удалось получить пользователя" << query.lastError();
        return user;
    }

    if (query.next())
    {
        QSqlRecord record = query.record();
        user.gitId  = record.value("Id").toInt();
        user.name   = record.value("Name").toString();
        user.gitUrl = record.value("WebUrl").toString();
    }

    return user;
}

bool DatabaseWorker::addUser(const UserData &data)
{
    if (containsUser(data.gitId)) return false;

    QSqlQuery query("INSERT INTO Users (Id, Name, WebUrl) "
                    "VALUES (?, ?, ?)");
    query.addBindValue(data.gitId);
    query.addBindValue(data.name);
    query.addBindValue(data.gitUrl);

    if (!query.exec())
    {
        qWarning() << "Не удалось добавить пользователя" << query.lastError();
        return false;
    }

    return true;
}

bool DatabaseWorker::updateUser(const UserData &data)
{
    if (!containsUser(data.gitId)) return false;

    QSqlQuery query("UPDATE Users "
                    "SET Name = ?, WebUrl = ? "
                    "WHERE Id=?");

    query.addBindValue(data.name);
    query.addBindValue(data.gitUrl);
    query.addBindValue(data.gitId);

    if (!query.exec())
    {
        qWarning() << "Не удалось обновить данные пользователя в БД" << query.lastError();
        return false;
    }

    return true;
}

bool DatabaseWorker::containsUser(const int &userId) const
{
    QSqlQuery query("SELECT count(Id) FROM Users WHERE Id=?");
    query.addBindValue(userId);

    if (!query.exec())
    {
        qWarning() << "Ошибка во время выполнения SELECT count(Id) FROM Users WHERE Id=?."
                   << query.lastError();
        return false;
    }
    query.next();
    return query.value(0).toBool();
}

void DatabaseWorker::init()
{
    if (!db.isOpen()) return;

    QSqlQuery query("CREATE TABLE IF NOT EXISTS \"PrivateKeys\" ("
                    "\"Id\"      INTEGER NOT NULL UNIQUE, "
                    "\"KeyName\" TEXT NOT NULL, "
                    "\"Key\"     TEXT NOT NULL UNIQUE, "
                    "PRIMARY KEY(\"Id\" AUTOINCREMENT)"
                    ")");
    if (!query.exec())
    {
        qWarning() << "Ошибка при попытке создания таблицы приватных ключей" << query.lastError();
    }

    query.prepare("CREATE TABLE IF NOT EXISTS \"Users\" ("
                  "\"Id\"          INTEGER NOT NULL UNIQUE, "
                  "\"Name\" TEXT NOT NULL, "
                  "\"WebUrl\"      TEXT NOT NULL, "
                  "PRIMARY KEY(\"Id\")"
                  ")");
    if (!query.exec())
    {
        qWarning() << "Ошибка при попытке создания таблицы проектов" << query.lastError();
    }

    query.prepare("CREATE TABLE IF NOT EXISTS \"Projects\" ("
                  "\"Id\"          INTEGER NOT NULL UNIQUE, "
                  "\"ProjectName\" TEXT NOT NULL, "
                  "\"WebUrl\"      TEXT NOT NULL, "
                  "\"LocalPath\"   TEXT, "
                  "\"PrivateKey\"  INTEGER, "
                  "\"UserId\"      INTEGER, "
                  "\"CreateTime\"  INTEGER, "
                  "PRIMARY KEY(\"Id\"), "
                  "FOREIGN KEY(\"PrivateKey\") REFERENCES \"PrivateKeys\"(\"Id\"), "
                  "FOREIGN KEY(\"UserId\") REFERENCES \"Users\"(\"Id\")"
                  ")");
    if (!query.exec())
    {
        qWarning() << "Ошибка при попытке создания таблицы " << query.lastError();
    }
}

#include "databaseworker.h"

DatabaseWorker::DatabaseWorker(QObject *parent)
    : QObject{parent}
{
    db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    db.setDatabaseName(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/database.sqlite");
    qDebug() << db.databaseName();

    db.open();

    init();
}

QList<Project> DatabaseWorker::getProjects() const
{
    QSqlQuery query("SELECT * FROM Projects ORDER BY Id");

    if (!query.exec())
    {
        qWarning() << "Не удалось получить список проектов" << query.lastError();
    }

    QList<Project> res;
    while (query.next())
    {
        QSqlRecord record = query.record();
        Project project;
        project.id              = record.value("Id").toInt();
        if (record.value("PrivateKey").isNull())
        {
            project.privateKeyId    = record.value("PrivateKey").toInt();
        }
        project.name            = record.value("ProjectName").toString();
        project.webUrl          = record.value("WebUrl").toString();
        project.pathToLocalRepo = record.value("LocalPath").toString();

        res.append(project);
    }

    return res;
}

bool DatabaseWorker::addProject(const Project &project)
{
    if (containsProject(project.id)) return false;

    QSqlQuery query("INSERT INTO Projects (Id, ProjectName, WebUrl, LocalPath, PrivateKey) "
                    "VALUES (?, ?, ?, ?, ?)");

    query.addBindValue(project.id);
    query.addBindValue(project.name);
    query.addBindValue(project.webUrl);
    query.addBindValue(project.pathToLocalRepo.isEmpty() ? QVariant() : project.pathToLocalRepo);
    query.addBindValue(project.privateKeyId ? project.privateKeyId.value() : QVariant());

    if (!query.exec())
    {
        qWarning() << "Не удалось добавить новый проект в БД" << query.lastError();
        return false;
    }

    return true;
}

bool DatabaseWorker::updateProject(const Project &project)
{
    if (!containsProject(project.id)) return false;

    QSqlQuery query("UPDATE Projects "
                    "SET ProjectName=:pName, WebUrl=:url, LocalPath=:localPath, PrivateKey=:privateKey"
                    "WHERE Id=:id");

    query.bindValue(":Id", project.id);
    query.bindValue(":ProjectName", project.name);
    query.bindValue(":WebUrl", project.webUrl);
    query.bindValue(":LocalPath", project.pathToLocalRepo.isEmpty() ? QVariant() : project.pathToLocalRepo);
    query.bindValue(":PrivateKey", project.privateKeyId ? project.privateKeyId.value() : QVariant());

    if (!query.exec())
    {
        qWarning() << "Не удалось добавить новый проект в БД" << query.lastError();
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

bool DatabaseWorker::containsProject(const int &projectId)
{
    QSqlQuery query("SELECT count(Id) FROM Projects WHERE Id=:id");
    query.bindValue(":id", projectId);

    if (!query.exec())
    {
        qWarning() << "Ошибка во время подсчёта количетсва проектов с индексом" << projectId;
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

bool DatabaseWorker::containsPrivateKey(const QString &key)
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

bool DatabaseWorker::containsPrivateKey(const int &keyId)
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

    query.prepare("CREATE TABLE IF NOT EXISTS \"Projects\" ("
                  "\"Id\"          INTEGER NOT NULL UNIQUE, "
                  "\"ProjectName\" TEXT NOT NULL, "
                  "\"WebUrl\"      TEXT NOT NULL, "
                  "\"LocalPath\"   TEXT, "
                  "\"PrivateKey\"  INTEGER, "
                  "PRIMARY KEY(\"ProjectId\"), "
                  "FOREIGN KEY(\"PrivateKey\") REFERENCES \"PrivateKeys\"(\"Id\")"
                  ")");
    if (!query.exec())
    {
        qWarning() << "Ошибка при попытке создания таблицы " << query.lastError();
    }
}

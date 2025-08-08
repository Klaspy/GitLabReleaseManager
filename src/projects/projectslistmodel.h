#ifndef PROJECTSLISTMODEL_H
#define PROJECTSLISTMODEL_H

#include <QGuiApplication>
#include <QAbstractListModel>
#include <QCursor>

#include <project.h>

class ProjectsListModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    enum Roles {
        ProjectRole = 0x101
    };

    explicit ProjectsListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE bool containsProject(int projectId);
    Q_INVOKABLE void addProject(int projectId, int privateKeyId);

signals:
    void projectAdded();
    void addProjectError(QString errorString);

private slots:
    void onGetProjectDone(ProjectData pData);
    void onGetProjectError(int projectId, Requester::RequestError error, QString note = "");

private:
    QList<Project*> projects;
    std::optional<int> tempProjectId {std::nullopt};
};

#endif // PROJECTSLISTMODEL_H

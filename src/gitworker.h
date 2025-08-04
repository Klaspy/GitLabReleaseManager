#ifndef GITWORKER_H
#define GITWORKER_H

#include <QObject>

class GitWorker : public QObject
{
    Q_OBJECT
public:
    explicit GitWorker(QObject *parent = nullptr);

signals:
};

#endif // GITWORKER_H

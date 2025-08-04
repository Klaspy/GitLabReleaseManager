#ifndef REQUESTER_H
#define REQUESTER_H

#include <QNetworkAccessManager>
#include <QObject>

class Requester : public QNetworkAccessManager
{
    Q_OBJECT
public:
    Requester(QObject *parent = nullptr);
};

#endif // REQUESTER_H

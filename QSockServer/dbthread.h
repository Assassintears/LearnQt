#ifndef DBTHREAD_H
#define DBTHREAD_H
#include <QThread>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QString>
#include "quedata.h"

class DBThread : public QThread
{
    Q_OBJECT
public:
    DBThread();
    virtual ~DBThread() override;

protected:
    void run() override;
};

#endif // DBTHREAD_H

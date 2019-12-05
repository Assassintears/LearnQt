#ifndef QUEDATA_H
#define QUEDATA_H

#include <QList>
#include <QByteArray>
#include <QMutex>

class ListData
{
public:
    ListData();
    virtual ~ListData();

public:
    void push(const QByteArray& array);//!插入一个数据
    QByteArray pop();//!弹出一个数据
    void pushBack(const QByteArray& array);//!尾部插入数据

private:
    static QMutex mutex;
    static QList<QByteArray> data;
};

static ListData g_Data;

#endif // QUEDATA_H

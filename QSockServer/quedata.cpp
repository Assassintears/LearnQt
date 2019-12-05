#include "quedata.h"
#include <QMutexLocker>

QMutex ListData::mutex;
QList<QByteArray> ListData::data;

ListData::ListData()
{}

ListData::~ListData()
{
    if (!ListData::data.isEmpty())
    {
        ListData::data.clear();
    }
}

void ListData::push(const QByteArray& array)
{
    QMutexLocker locker(&ListData::mutex);
    ListData::data.insert(0, array);
}
QByteArray ListData::pop()
{
    if (!ListData::data.isEmpty())
    {
        QMutexLocker locker(&ListData::mutex);
        QByteArray array = ListData::data.last();
        ListData::data.removeLast();
        return array;
    }
    else
    {
        return QByteArray();
    }
}

void ListData::pushBack(const QByteArray& array)
{
    QMutexLocker locker(&ListData::mutex);
    ListData::data.append(array);
}

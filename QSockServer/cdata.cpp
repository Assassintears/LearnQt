#include "cdata.h"
#include <QMutexLocker>
#include <QVector>
#include <QDebug>

QMutex CData::mutex;
QList<QByteArray> CData::data;

CData::CData()
{}

CData::~CData()
{
    if (!CData::data.isEmpty())
    {
        CData::data.clear();
    }
}

void CData::push(const QByteArray& array)
{
    QMutexLocker locker(&CData::mutex);
    CData::data.insert(0, array);
}
QByteArray CData::pop()
{
    if (!CData::data.isEmpty())
    {
        QMutexLocker locker(&CData::mutex);
        QByteArray array = CData::data.last();
        CData::data.removeLast();
        return array;
    }
    else
    {
        return QByteArray();
    }
}

void CData::pushBack(const QByteArray& array)
{
    QMutexLocker locker(&CData::mutex);
    CData::data.append(array);
}

void CData::parseJson(QByteArray buf)
{
    QJsonParseError json_error;
    QJsonDocument doc(QJsonDocument::fromJson(buf, &json_error));
    if (QJsonParseError::NoError == json_error.error)      //! json有效
    {
        QJsonValue coalID = doc["MeiChangID"];
        QJsonValue darray = doc["ZValue"];
        QJsonValue fx = doc["fX"];
        QJsonValue ScanIndex = doc["ScanIndex"];
        if (coalID.isUndefined() || darray.isUndefined() ||
                fx.isUndefined() || ScanIndex.isUndefined())
        {
            return;
        }

        int id;             //! 煤场ID
        int x;              //! x坐标
        int index;          //! 扫描计数
        QJsonArray arr;
        if (coalID.isDouble())
        {
            id = coalID.toInt();
        }
        else if (coalID.isString())
        {
            id = coalID.toString().toInt();
        }
        else {
            return;
        }

        if (fx.isDouble())
        {
            x = fx.toInt();
        }
        else if (fx.isString())
        {
            x = fx.toString().toInt();
        }
        else {
            return;
        }

        if (ScanIndex.isDouble())
        {
            index = ScanIndex.toInt();
        }
        else if (ScanIndex.isString())
        {
            index = ScanIndex.toString().toInt();
        }
        else {
            return;
        }
        if (darray.isArray())
        {
            arr = darray.toArray();
        }
        else {
            return;
        }
        QVector<int> vec;
        int size = arr.size();
        for (int it = 0; it < size; ++it)
        {
            vec.push_back(arr[it].toInt());
        }
        emit jsonData(doc.toJson());
        emit oneRowData(id, index, x, vec);
    }
    return;

}

#ifndef QUEDATA_H
#define QUEDATA_H

#include <QList>
#include <QByteArray>
#include <QMutex>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>

class CData : public QObject
{
    Q_OBJECT
public:
    CData();
    virtual ~CData();

signals:
    void oneRowData(int coalID, int scanIndx, int x, QVector<int> vec);
    void jsonData(QString);

public slots:
    void parseJson(QByteArray);

public:
    void push(const QByteArray& array);//!插入一个数据
    QByteArray pop();//!弹出一个数据
    void pushBack(const QByteArray& array);//!尾部插入数据

private:
    static QMutex mutex;
    static QList<QByteArray> data;
};

#endif // QUEDATA_H

#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
#include <QVector>
#include <QVector3D>
#include <QMutex>
#include <QMutexLocker>
#include <QHash>
#include <QTimer>
#include <QRegExp>

#define  STRINGIFY(x) #x
#define  TOSTRING(x) STRINGIFY(x)
#define  AT __FILE__ " : " TOSTRING(__LINE__)

//! 数据库的一些操作
class DataBase : public QObject
{
    Q_OBJECT
public:
    DataBase(const QString& connectName);
    virtual ~DataBase();

signals:
    void customErr(QString);                                            //! 一些错误说明
    //! 0: 正常， 1：断开，2：数据库中表不存在
    void State(int);                                                    //! 数据库状态
    void needInit();

public slots:
    void insertIntoTable(int coalID, int indx,
                         int x, QVector<int> vec);                      //! 写入数据库
    void reconnectDB();                                                 //! 数据库断开重连
    void initDB();                                                      //! 初始化数据库
private:
    QSqlDatabase m_dataBase;
    QMutex m_mutex;
    QString m_connectName;
    QHash<int, QString> m_coalInfo; //! 煤场信息：key：煤场ID，value：煤场实时数据表名称
    QString getError(const char* location, const char* msg = "");

public:
    bool m_isInited;
    bool m_isBreak;     //! 数据库是否断开

};


#endif // DATABASE_H

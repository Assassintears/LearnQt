#include <database.h>
#include <QThread>
#include <QTime>

DataBase::DataBase(const QString& connectName)
    :QObject (nullptr),
      m_mutex{}
{
    m_isBreak = true;          //! 默认断开
    m_isInited = false;         //! 默认未初始化
    //!开始连接数据库
    m_dataBase = QSqlDatabase::addDatabase("QODBC", connectName);
    m_dataBase.setHostName("192.168.2.136");
    m_dataBase.setDatabaseName("DB");//!如果是QODBC驱动，此处应该是数据源名称
    m_dataBase.setUserName("sa");
    m_dataBase.setPassword("sa");
    m_dataBase.setPort(1433);
    m_connectName = connectName;
    connect(this, &DataBase::needInit, this, &DataBase::initDB);

}

DataBase::~DataBase()
{
}

void DataBase::reconnectDB()
{
    if (!m_isBreak)
    {
        return;
    }
    m_dataBase.close();
    if (m_dataBase.open())
    {
        if (!m_isInited)
        {
            emit needInit();
        }
        emit customErr("连上数据库\n");
        emit State(0);
        return;
    }
    else {
        emit customErr("正在尝试连接数据库\n");
        QThread::sleep(1);
    }
}

void DataBase::initDB()
{
    if (m_isInited)
    {
        return;
    }
    if (!m_dataBase.isOpen())
    {
        if (!m_dataBase.open())
        {
            emit State(1);
            return;
        }
    }
     QSqlQuery query(m_dataBase);
     QString sql = "SELECT * FROM InitTable";
     query.exec(sql);
     if (!query.isActive())
     {
         emit State(1);
         return;
     }

     while (query.next()) {
         QSqlRecord rec = query.record();
         //! 取总表中的煤场ID和实时数据表
        int id = rec.value(rec.indexOf("SiteID")).toInt();
        QString name = rec.value(rec.indexOf("TableName")).toString();
        emit customErr(name);
        m_coalInfo[id] = name;
     }
     m_isInited = true;
     query.clear();
     m_dataBase.close();
}

void DataBase::insertIntoTable(int coalID, int indx, int x, QVector<int> vec)
{
    if (!m_isInited)
    {
        emit customErr("未初始化");
        emit needInit();
        return;
    }
    if (m_isBreak)
    {
        emit customErr("数据库断开连接");
        emit State(1);
        return;
    }
    QString table = m_coalInfo.value(coalID, "");
    if (!m_dataBase.isOpen())
    {
        if (!m_dataBase.open())
        {
            emit State(1);
            return;
        }
    }

    if (table.isEmpty())                                //! 没有对应的表
    {
        QString err = "没有对应的表 " + getError(AT);
        emit State(2);
        emit customErr(err);
        return;
    }

    QSqlQuery query(m_dataBase);
    QString sql = QString("INSERT INTO %1(x) VALUES (%2)").arg(table).arg(x);
    query.exec(sql);
    if (query.isActive())
    {
        query.clear();
    }
    else
    {
        if (!m_dataBase.isOpen())
        {
            emit State(1);
        }
    }
    sql = QString("UPDATE %1 SET").arg(table);
    for (int i = 0; i < vec.size(); ++i)
    {
        sql += QString(" y%1=%2,").arg(i).arg(vec[i]);
    }
    sql += QString(" time=CONVERT(varchar,GETDATE(),120)  where x=%1").arg(x);
    query.exec(sql);
    if (query.isActive())
    {
        query.clear();
        m_dataBase.close();
        return;
    }
    else {
        emit State(1);
        m_dataBase.close();
        return;
    }
}

QString DataBase::getError(const char* location, const char* fileName)
{
    QString file = QString::fromStdString(fileName);
    QString loc = QString::fromStdString(location);
    if (file.endsWith(".cpp"))
    {
        QStringList list = file.split(QRegExp("\\+"));
        file = list.at(list.size() - 1);
    }

    return file + QString(" : ") + loc;
}

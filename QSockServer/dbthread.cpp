#include "dbthread.h"


DBThread::DBThread()
{}

DBThread::~DBThread()
{
    requestInterruption();
    quit();
    wait();
}

void DBThread::run()
{
    forever
    {
        if (!isInterruptionRequested())
        {
            QByteArray array = g_Data.pop();
            if (!array.isEmpty())
            {
                //!解析json
                QJsonParseError json_error;
                QJsonDocument doc = QJsonDocument::fromJson(array, &json_error);
                if (QJsonParseError::NoError == json_error.NoError)
                {
                    if (doc.isObject())
                    {
                        QJsonObject obj = doc.object();
                    }
                }
            }
            this->sleep(100);
        }
    }
}

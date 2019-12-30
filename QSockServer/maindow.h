#ifndef MainWidow_H
#define MainWidow_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QPushButton>
#include <QLineEdit>
#include <QGroupBox>
#include <QTextEdit>
#include <QLabel>
#include <QList>
#include <QComboBox>
#include <QTimer>
#include <QThread>

#include <cdata.h>
#include <database.h>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif



class MainWidow : public QMainWindow
{
    Q_OBJECT

public:
    MainWidow(QWidget *parent = nullptr);
    ~MainWidow();

private:
    QList<QTcpSocket*> m_sockClient;
    QTcpServer* m_sockServer;
    QTcpSocket* currentClient;

    QPushButton* m_bShut;//!断开连接
//    QPushButton* m_bSend;//!发送
    QLineEdit* m_lineIP;//!IP编辑
    QLineEdit* m_linePort;//!端口号编辑
    QGroupBox* m_recWnd;//!接收窗口
    QGroupBox* m_sendWnd;//!发送窗口
    QTextEdit* text;
    QTextEdit* sendText;
    QGroupBox* netSet;//!网络设置

    QLabel* m_labelServerIP;
    QLabel* m_lablePort;

    QComboBox* m_comBoxConnected;   //!下拉列表，显示已连接的客户端
    QTimer* timer;                  //! 重连数据库

    QThread* dataThread;    //! 数据处理线程
    QThread* dbThread;      //! 数据库操作线程
    CData* m_cdata;
    DataBase* m_database;

    void createComs();
    void SingnalandSlots();
    void layout();
    void doTimeOut();

signals:
    void startWork();
    void DataBuffer(QByteArray buf);    //! 读取到数据
    void needInitDB();                  //! 初始化数据库信号
    void reconnectDB();                 //! 重连数据库

public slots:
    void newConnect();
    void readData();
    void disconnectedSlot();
    void listenClient();
//    void senData();
//    void createThread();
//    void printID(Qt::HANDLE id);
    void printErr(QString);             //! 打印错误信息
    void doState(int);                  //! 处理数据库返回的状态

};

#endif // MainWidow_H

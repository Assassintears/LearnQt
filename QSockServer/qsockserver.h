#ifndef QSOCKSERVER_H
#define QSOCKSERVER_H

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
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>

#include "quedata.h"
#include "dbthread.h"



class QSockServer : public QMainWindow
{
    Q_OBJECT

public:
    QSockServer(QWidget *parent = nullptr);
    ~QSockServer();

private:
    QList<QTcpSocket*> m_sockClient;
    QTcpServer* m_sockServer;
    QTcpSocket* currentClient;

    QPushButton* m_bShut;//!断开连接
    QPushButton* m_bSend;//!发送
    QLineEdit* m_lineIP;//!IP编辑
    QLineEdit* m_linePort;//!端口号编辑
    QGroupBox* m_recWnd;//!接收窗口
    QGroupBox* m_sendWnd;//!发送窗口
    QTextEdit* text;
    QTextEdit* sendText;
    QGroupBox* netSet;//!网络设置

    QLabel* m_labelServerIP;
    QLabel* m_lablePort;

    QComboBox* m_comBoxConnected;//!下拉列表，显示已连接的客户端
    QTimer* timer;

    void createComs();
    void SingnalandSlots();
    void layout();

private:
    DBThread workThread;

public slots:
    void newConnect();
    void readData();
    void disconnectedSlot();
    void listenClient();
    void senData();
};

#endif // QSOCKSERVER_H

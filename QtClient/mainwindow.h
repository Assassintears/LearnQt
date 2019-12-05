#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QPushButton>
#include <QLineEdit>
#include <QGroupBox>
#include <QTextEdit>
#include <QLabel>

class QClient : public QMainWindow
{
    Q_OBJECT

public:
    QClient(QWidget *parent = nullptr);
    virtual ~QClient();

private:
    QTcpSocket* m_sockClient;

    QPushButton* m_bShut;//!断开连接
    QPushButton* m_bSend;//!发送
    QLineEdit* m_lineIP;//!IP编辑
    QLineEdit* m_linePort;//!端口号编辑
    QLineEdit* m_lineLocalIP;//!本机IP
    QGroupBox* m_recWnd;//!接收窗口
    QGroupBox* m_sendWnd;//!发送窗口
    QTextEdit* text;
    QTextEdit* sendText;
    QGroupBox* netSet;//!网络设置

    QLabel* m_labelServerIP;
    QLabel* m_lablePort;
    QLabel* m_labelLocalIP;

    void createComs();
    void SingnalandSlots();
    void layout();
    void readData();
    void sendData();

public slots:
    void connectToServer();


};

#endif // MAINWINDOW_H

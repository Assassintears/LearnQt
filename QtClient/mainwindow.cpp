#include "mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRegExpValidator>
#include <QAbstractSocket>
#include <QHostInfo>

QClient::QClient(QWidget *parent)
    : QMainWindow(parent)
{
    createComs();
    layout();
    SingnalandSlots();
}

QClient::~QClient()
{

}

void QClient::createComs()
{
    m_sockClient = new QTcpSocket;
    m_bShut = new QPushButton(QStringLiteral("Connect"), this);
    m_bSend = new QPushButton(tr("Send"), this);
    m_lineIP = new QLineEdit;
    m_lineIP->setValidator(new QRegExpValidator(QRegExp("[0-9]{3}\\.[0-9]{3}\\.[0-9]+\\.[0-9]+$")));
    m_linePort = new QLineEdit;
    m_linePort->setValidator(new QRegExpValidator(QRegExp("[0-9]{3,5}$")));
    m_recWnd = new QGroupBox(QStringLiteral("Receive Wind"), this);
    m_sendWnd = new QGroupBox(QStringLiteral("Send Wind"), this);
    text = new QTextEdit;
    netSet = new QGroupBox(QStringLiteral("Net"), this);
    m_lablePort = new QLabel(QStringLiteral("Port"), this);
    m_labelServerIP = new QLabel(QStringLiteral("Server IP"), this);
    m_lineLocalIP = new QLineEdit;
    m_labelLocalIP = new QLabel(QStringLiteral("Local IP"), this);
    sendText = new QTextEdit;

    m_lineIP->setText("192.168.2.218");
    m_linePort->setText("7171");

    //!获取本机IP
    QString localHostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHostName);
    if (!info.addresses().empty())
    {
        foreach(QHostAddress address,info.addresses())
        {
            if(address.protocol() == QAbstractSocket::IPv4Protocol)
            {
                m_lineLocalIP->setText(address.toString());
                break;
            }
        }
    }
}

void QClient::layout()
{
    QVBoxLayout* mainLay = new QVBoxLayout;
    QHBoxLayout* upLay = new QHBoxLayout;
    QHBoxLayout* downLay = new QHBoxLayout;
    QHBoxLayout* left = new QHBoxLayout;
    left->addWidget(text);
    m_recWnd->setLayout(left);
    QVBoxLayout* right = new QVBoxLayout;
    right->addWidget(m_labelServerIP);
    right->addWidget(m_lineIP);
    right->addWidget(m_lablePort);
    right->addWidget(m_linePort);
    right->addWidget(m_labelLocalIP);
    right->addWidget(m_lineLocalIP);
    right->addWidget(m_bShut);
    netSet->setLayout(right);
    upLay->addWidget(m_recWnd);
    upLay->addWidget(netSet);

    downLay->addWidget(sendText);
    downLay->addWidget(m_bSend);
    m_sendWnd->setLayout(downLay);
    mainLay->addLayout(upLay);
    mainLay->addWidget(m_sendWnd);

    QWidget* cent = new QWidget;
    cent->setLayout(mainLay);
    this->setCentralWidget(cent);
}

void QClient::SingnalandSlots()
{
    connect(m_bShut, &QPushButton::clicked, this, &QClient::connectToServer);
    connect(m_sockClient, &QTcpSocket::readyRead, this, &QClient::readData);
    connect(m_bSend, &QPushButton::clicked,this, &QClient::sendData);
}

void QClient::connectToServer()
{
    QString line = m_bShut->text();
    QString ip = m_lineIP->text();
    quint16 port = m_linePort->text().toUShort();
    m_sockClient->connectToHost(ip, port, QIODevice::ReadWrite, QAbstractSocket::IPv4Protocol);
    m_sockClient->waitForConnected(1000);
    if (QAbstractSocket::ConnectingState == m_sockClient->state() ||
            QAbstractSocket::ConnectedState == m_sockClient->state())
    {
        text->clear();
        text->setPlainText("连接成功\n");
    }
    else
    {
        text->clear();
        text->setPlainText("连接失败\n");
    }
}

void QClient::readData()
{
    QString str = QString(m_sockClient->readAll());
    text->clear();
    text->setPlainText(str);
}
void QClient::sendData()
{
    QString str = sendText->toPlainText();
    m_sockClient->write(str.toLatin1());
}

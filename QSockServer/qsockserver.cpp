#include "qsockserver.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "quedata.h"

QSockServer::QSockServer(QWidget *parent)
    : QMainWindow(parent)
{
    createComs();
    layout();
    SingnalandSlots();

    workThread.start();
}

QSockServer::~QSockServer()
{

}

void QSockServer::createComs()
{
    m_sockServer = new QTcpServer;
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
    sendText = new QTextEdit;
    m_comBoxConnected = new QComboBox;

    m_lineIP->setText("192.168.2.218");
    m_linePort->setText("7171");

    timer = new QTimer;
    timer->start(1000);
}

void QSockServer::layout()
{
    QHBoxLayout* left = new QHBoxLayout;
    left->addWidget(text);
    m_recWnd->setLayout(left);

    QVBoxLayout* right = new QVBoxLayout;
    right->addWidget(m_labelServerIP);
    right->addWidget(m_lineIP);
    right->addWidget(m_lablePort);
    right->addWidget(m_linePort);
    right->addWidget(m_bShut);
    right->addWidget(m_comBoxConnected);
    netSet->setLayout(right);

    QHBoxLayout* upLay = new QHBoxLayout;
    upLay->addWidget(m_recWnd);
    upLay->addWidget(netSet);

    QHBoxLayout* downLay = new QHBoxLayout;
    downLay->addWidget(sendText);
    downLay->addWidget(m_bSend);
    m_sendWnd->setLayout(downLay);

    QVBoxLayout* mainLay = new QVBoxLayout;
    mainLay->addLayout(upLay);
    mainLay->addWidget(m_sendWnd);

    QWidget* cent = new QWidget;
    cent->setLayout(mainLay);
    this->setCentralWidget(cent);
}

void QSockServer::SingnalandSlots()
{
    connect(m_sockServer, &QTcpServer::newConnection, this, &QSockServer::newConnect);
    connect(m_bShut, &QPushButton::clicked, this, &QSockServer::listenClient);
    connect(m_bSend, &QPushButton::clicked, this, &QSockServer::senData);
}

void QSockServer::newConnect()
{
    currentClient = m_sockServer->nextPendingConnection();
    m_sockClient.append(currentClient);
    m_comBoxConnected->addItem(currentClient->peerAddress().toString());
    connect(currentClient, &QTcpSocket::readyRead, this, &QSockServer::readData);
    connect(currentClient, &QTcpSocket::disconnected, this, &QSockServer::disconnectedSlot);
}

void QSockServer::readData()
{
    text->clear();

    for(QList<QTcpSocket*>::iterator it = m_sockClient.begin(); it != m_sockClient.end(); ++it)
    {

       QByteArray buffer = (*it)->readAll();
       if (!buffer.isEmpty())
       {
           QString ip = (*it)->peerAddress().toString();
           QString all = ip + ": \n" + QString(buffer) + "\n";
           text->setPlainText(all);
           g_Data.push(buffer);
       }
    }

}
void QSockServer::disconnectedSlot()
{
    for(QList<QTcpSocket*>::iterator it = m_sockClient.begin(); it != m_sockClient.end(); ++it)
    {

        if (QAbstractSocket::UnconnectedState == (*it)->state())
        {
//            qDebug() << (*it)->peerAddress().toString() << " Shut\n";
            m_comBoxConnected->removeItem(
                        m_comBoxConnected->findText(
                            (*it)->peerAddress().toString()));

            //!当客户端关闭后，QT自动处理客户端遗留问题，不需要手动管理删除
//            (*it)->destroyed();
//            it = m_sockClient.erase(it);

        }
    }
}

void QSockServer::listenClient()
{
    QString ip = m_lineIP->text();
    quint16 port = m_linePort->text().toUShort();
    QHostAddress addr(ip);
    m_sockServer->listen(addr, port);
}

void QSockServer::senData()
{
    QString currentIP = m_comBoxConnected->currentText();
    QString currentStr = sendText->toPlainText();
    if (!currentIP.isEmpty() & !currentStr.isEmpty())
    {
        foreach (QTcpSocket* sock, m_sockClient)
        {
            if (sock->peerAddress().toString() == currentIP)
            {
                sock->write(currentStr.toLatin1());
                break;
            }
        }
    }
}

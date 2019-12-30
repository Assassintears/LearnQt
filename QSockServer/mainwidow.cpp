#include "maindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QHostInfo>
#include "cdata.h"

MainWidow::MainWidow(QWidget *parent)
    : QMainWindow(parent)
{
    QIcon* icon = new QIcon(":/resorces/server.ico");
    if (!icon->isNull())
    {
        this->setWindowIcon(*icon);
    }
    createComs();
    layout();
    SingnalandSlots();
    emit needInitDB();
}

MainWidow::~MainWidow()
{
    dataThread->quit();
    dataThread->wait();
    dbThread->quit();
    dbThread->wait();
}

void MainWidow::createComs()
{
    m_sockServer = new QTcpServer;
    m_bShut = new QPushButton(QStringLiteral("Connect"), this);
    m_lineIP = new QLineEdit;
    m_lineIP->setValidator(new QRegExpValidator(QRegExp("[0-9]{3}\\.[0-9]{3}\\.[0-9]+\\.[0-9]+$")));
    m_linePort = new QLineEdit;
    m_linePort->setValidator(new QRegExpValidator(QRegExp("[0-9]{3,5}$")));
    m_recWnd = new QGroupBox(QStringLiteral("Receive Wnd"), this);
    m_sendWnd = new QGroupBox(tr("Message"), this);
    text = new QTextEdit;
    netSet = new QGroupBox(QStringLiteral("Net"), this);
    m_lablePort = new QLabel(QStringLiteral("Port"), this);
    m_labelServerIP = new QLabel(QStringLiteral("Server IP"), this);
    sendText = new QTextEdit;
    m_comBoxConnected = new QComboBox;

    m_cdata = new CData;
    m_database = new DataBase("database");
    dataThread = new QThread;
    dbThread = new QThread;
    m_cdata->moveToThread(dataThread);
    m_database->moveToThread(dbThread);
    dataThread->start();
    dbThread->start();

    timer = new QTimer;
    timer->start(1000);

    //!获取本机IP
    QString localHostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHostName);
    if (!info.addresses().empty())
    {
        foreach(QHostAddress address,info.addresses())
        {
            if(address.protocol() == QAbstractSocket::IPv4Protocol)
            {
                m_lineIP->setText(address.toString());
                m_linePort->setText("7171");
                break;
            }
        }
    }
}

void MainWidow::layout()
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
//    downLay->addWidget(m_bSend);
    m_sendWnd->setLayout(downLay);

    QVBoxLayout* mainLay = new QVBoxLayout;
    mainLay->addLayout(upLay);
    mainLay->addWidget(m_sendWnd);

    QWidget* cent = new QWidget;
    cent->setLayout(mainLay);
    this->setCentralWidget(cent);
}

void MainWidow::SingnalandSlots()
{
    //! 注册元数据
    qRegisterMetaType<QVector<int> > ("Zvalue");
    qRegisterMetaType<QByteArray> ("ByteArray");

    connect(dataThread, &QThread::finished, m_cdata, &QObject::deleteLater);
    connect(dbThread, &QThread::finished, m_database, &QObject::deleteLater);
    connect(m_sockServer, &QTcpServer::newConnection, this, &MainWidow::newConnect);
    connect(m_bShut, &QPushButton::clicked, this, &MainWidow::listenClient);
    connect(this, &MainWidow::DataBuffer, m_cdata, &CData::parseJson, Qt::DirectConnection);
    connect(m_cdata, &CData::oneRowData, m_database, &DataBase::insertIntoTable);
    connect(m_database, &DataBase::customErr, this, &MainWidow::printErr);
    connect(timer, &QTimer::timeout, this, &MainWidow::doTimeOut);
    connect(this, &MainWidow::reconnectDB, m_database, &DataBase::reconnectDB);
    connect(m_database, &DataBase::State, this, &MainWidow::doState);
    connect(m_cdata, &CData::jsonData, text, &QTextEdit::insertPlainText);
}

void MainWidow::newConnect()
{
    currentClient = m_sockServer->nextPendingConnection();
    m_sockClient.append(currentClient);
    m_comBoxConnected->addItem(currentClient->peerAddress().toString());
    connect(currentClient, &QTcpSocket::readyRead, this, &MainWidow::readData);
    connect(currentClient, &QTcpSocket::disconnected, this, &MainWidow::disconnectedSlot);
}

void MainWidow::readData()
{
    for(QList<QTcpSocket*>::iterator it = m_sockClient.begin(); it != m_sockClient.end(); ++it)
    {

        if ((*it)->isReadable())
        {
            QByteArray buffer = (*it)->readAll();
            if (!buffer.isEmpty())
            {
                emit DataBuffer(buffer);
            }
        }
    }

}
void MainWidow::disconnectedSlot()
{
    for(QList<QTcpSocket*>::iterator it = m_sockClient.begin(); it != m_sockClient.end(); ++it)
    {

        if (QAbstractSocket::UnconnectedState == (*it)->state())
        {
            m_comBoxConnected->removeItem(
                        m_comBoxConnected->findText(
                            (*it)->peerAddress().toString()));

            //!当客户端关闭后，QT自动处理客户端遗留问题，不需要手动管理删除
//            (*it)->destroyed();
//            it = m_sockClient.erase(it);

        }
    }
}

void MainWidow::listenClient()
{
    QString ip = m_lineIP->text();
    quint16 port = m_linePort->text().toUShort();
    QHostAddress addr(ip);
    m_sockServer->listen(addr, port);
}

void MainWidow::printErr(QString s)
{
    QString tmp = s + "\r";
    sendText->insertPlainText(tmp);
}

void MainWidow::doState(int state)
{
    qDebug() << "返回状态 " << state << "\n";
    if (0 == state)
    {
        m_database->m_isBreak = false;
    }
    else if (1 == state)
    {
        m_database->m_isBreak = true;
    }
    else if (2 == state)
    {
    }
}

void MainWidow::doTimeOut()
{
    if (m_database->m_isBreak)
    {
        emit reconnectDB();
    }
}

//void MainWidow::senData()
//{
//    QString currentIP = m_comBoxConnected->currentText();
//    QString currentStr = sendText->toPlainText();
//    if (!currentIP.isEmpty() & !currentStr.isEmpty())
//    {
//        foreach (QTcpSocket* sock, m_sockClient)
//        {
//            if (sock->peerAddress().toString() == currentIP)
//            {
//                sock->write(currentStr.toLatin1());
//                break;
//            }
//        }
//    }
//}


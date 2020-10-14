#include "server.h"
#include "ui_server.h"


#include <QFileInfo>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

Server::Server(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);
    //初始化
    path = nullptr;
    server = new QTcpServer(this);
    timer = new QTimer();
    fileSize = 0; //文件大小

    this->setFixedSize(458,340);
    this->setWindowTitle("服务器 8888");
    this->setWindowIcon(QIcon(":/img/server.png"));
    server->listen(QHostAddress("127.1.1.1"),8888); //监听

    /*（1. Qt5版本的connect，这种会语法判断）*/
    connect(ui->sendFile,&QPushButton::clicked,this,&Server::sendFileFun);
    /*(2.Qt4版本connect，这种是宏定义的，不会语法判断,还可以带参数)*/
    connect(server,SIGNAL(newConnection()),this,SLOT(acceptConn()));
    /*（3.槽函数 lambda表达式）选择文件连接*/
    connect(ui->selectFile,&QPushButton::clicked,this,[=](){
        path = QFileDialog::getOpenFileName(this,tr("需要传输的文件"));
        if(path.isEmpty())
        {
            QMessageBox::information(this,"提示","当前未选择文件！");
            ui->sendFile->setEnabled(false);     //发送按钮关闭
            return ;
        }
        ui->sendMsg->append("当前文件： "+ path);
        ui->sendFile->setEnabled(true);     //发送按钮打开
    });
    /*定时器超时连接*/
    connect(timer,&QTimer::timeout,this,&Server::timerOutSendFile);

}

Server::~Server()
{
    delete ui;
}


/*
 *  功能：槽函数，用于点击发送处理文件
*/
void Server::sendFileFun()
{

    if(!conn->isValid())
    {
        this->ui->sendMsg->append("套接字无效！");
        conn = nullptr;
        ui->sendFile->setEnabled(false);
        ui->selectFile->setEnabled(false);

        return;
    }

    QString info;   //文件信息
    this->file.setFileName(this->path);
    if(!file.open(QIODevice::ReadOnly)) //打开文件
    {
        QMessageBox::critical(this,"错误","文件打开失败！");
        return;
    }
    getFileInfo(info);  //获得文件信息，发头文件给客户端

    qint64 len = conn->write(info.toUtf8());
    conn->waitForBytesWritten();    // 等待数据发送完毕
    if(len > 0)
    {
        // 预防粘包,使用定时器延迟一秒发送接下来的数据。
        timer->start(1000);
     }
    else
    {
        file.close();
    }

}




/*
 * 功能：接收客户端套接字对象，并且实例化通信套接字
*/
void Server::acceptConn()
{
    this->conn = this->server->nextPendingConnection();

    //获得客户端IP和端口
    QString ip = conn->peerAddress().toString();
    qint16 port = conn->peerPort();
    this->ui->sendMsg->append(QString("系统消息：有新的连接.Ip:%1,Port:%2").arg(ip).arg(port));

    //判断conn 是否有效
    if(!conn->isValid())
    {
        this->ui->sendMsg->append("连接错误！");
        conn = nullptr;
        return;

    }

    //当有客户端连接才能进行选择文件
    ui->selectFile->setEnabled(true);


    //连接通信套接字，不过该项目里客户端，不会发送数据，所以这里连不连接都一样。
    connect(conn,&QTcpSocket::readyRead,this,[=]()
    {
        ui->sendMsg->append("Ta："+ conn->readAll()); //接收数据显示
    });
}




/*
 * 功能：发送文件数据（定时器过时槽函数）
*/
void Server::timerOutSendFile()
{
    qint64 sendSize = 0;        //发送了 多少数据
    qint64 len = 0;           //当前发送的数据
    do
    {
        char buf[2 * 1014] = {0};
        len = 0;
        len = file.read(buf,sizeof buf);    //这里的len是读取文件的数据多少
        len = conn->write(buf,len);     //这里len是写入了套接字的多少
        sendSize += len;
    }while(len > 0);

    //到此写完套接字

    if(sendSize == fileSize)
    {
        ui->sendMsg->append("文件发送完成");
        file.close();
    }
    else
    {
          ui->sendMsg->append("发送失误");
    }

    timer->stop();  //定时器停止

}




/*
    功能：获得文件名和文件大小
    参数：传入字符串获得文件名##文件大小
*/
void Server::getFileInfo(QString &info)
{
    if(this->path.isEmpty())
        return;
    QFileInfo finfo(this->path);
    fileSize = finfo.size();
    //long long int 如何赚字符串呢
    info = finfo.fileName()+"##"+QString::number(finfo.size());
}

#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include <QFile>
#include <QTimer>


namespace Ui {
class Server;
}

class Server : public QWidget
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = nullptr);
    ~Server();

public slots:  //槽函数
    void sendFileFun(void); //发送文件
    void acceptConn(void);  //接收连接
    void timerOutSendFile(void);    //定时器触发槽函数

protected:
    void getFileInfo(QString & info);
private:
    Ui::Server *ui;
    QString path;         //文件路径
    qint64 fileSize;      //文件大小
    QTcpServer * server;  //监听套接字
    QTcpSocket * conn;    //通信套接字
    QFile file;           //文件
    QTimer * timer;       //定时器
};

#endif // SERVER_H

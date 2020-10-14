#ifndef CLINET_H
#define CLINET_H

#include <QWidget>
#include <QTcpSocket>
#include <QFile>


namespace Ui {
class Clinet;
}

class Clinet : public QWidget
{
    Q_OBJECT

public:
    explicit Clinet(QWidget *parent = nullptr);
    ~Clinet();

private:
    Ui::Clinet *ui;
    QTcpSocket * conne;     //通信套接字
    QFile file;             //文件对象
    qint64 fileSize;        //文件大小
    qint64 recSize;         //已接受的文件大小
    QString filename;       //文件名字
    bool flag;              //数据头部标志，false表示第一次接收

};

#endif // CLINET_H

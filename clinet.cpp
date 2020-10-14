#include "clinet.h"
#include "ui_clinet.h"

#include <QMessageBox>
#include <QHostAddress>
#include <QDebug>

Clinet::Clinet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Clinet)
{
    ui->setupUi(this);

    fileSize = 0;           //文件大小
    recSize = 0;
    flag = false;           //标志
    filename = nullptr;     //文件名
    conne = new QTcpSocket(this);

    this->setFixedSize(438,528);
    this->setWindowTitle("客户端");
    this->setWindowIcon(QIcon(":/img/clinet.png"));
    ui->sIp->setText("127.1.1.1");
    ui->sPort->setText("8888");

    /*连接服务器按钮*/
    connect(ui->linkServ,&QPushButton::clicked,this,[=]()
    {
        if(ui->sIp->text().isEmpty() || ui->sPort->text().isEmpty())
        {
            QMessageBox::critical(this,"警告","iP 地址和端口不得为空");
            flag = false;
            return ;
        }
        ui->proBar->setValue(0);

        conne->abort(); //取消已连接的端口
        conne->connectToHost(QHostAddress(ui->sIp->text()),ui->sPort->text().toInt());

    });


    //客户端接收数据
    connect(conne,&QTcpSocket::readyRead,this,[=]()
    {
        /*客户端的核心接收文件就从这里开始了*/
            QByteArray buf = conne->readAll();  //发送过来的就字节读取到缓冲区

            /*判断是不是头文件,服务器那边已做粘包处理*/
            if(false == flag)
            {
                flag = true;
                filename = QString(buf).section("##",0,0);
                fileSize = QString(buf).section("##",1,1).toInt();
                qDebug() << filename << fileSize;

                file.setFileName(filename);
                if(!file.open(QIODevice::WriteOnly))
                {
                    //打开失败，相关变量初始化
                    fileSize = 0;
                    recSize = 0;
                    filename = "";
                    flag = false;

                    QMessageBox::warning(this,"错误","创建文件失败");
                    return;
                }

               //设置进度条，本人觉得最有趣的地方
               /*
                * 这种是以百分比的进度条,每次增加按单体占总大小的百分比
                * 例如：ui->proBar->setValue(单体/总体*100);
                * 当然这样计算肯定比数值型的麻烦一点，不过确实是百分比的
                *
                */
                ui->proBar->setMinimum(0);
                ui->proBar->setMaximum(100);


               /*
                      另一种是纯数值型的
                    ui->proBar->setMinimum(0);
                    ui->proBar->setMaximum(最大值);
                    这种每次增加都往ui->proBar->value()即基础上加
                    即：ui->proBar->setValue(ui->proBar->value()+Number);
                */

                ui->recMsg->append(QString("正在接收文件：%1").arg(filename));
            }
            else    //目标文件，这里应该是第二次读取套接字，所以上来就直接写入就好了
            {
                //写入数据核心部分
                qint64 len = file.write(buf);
                recSize += len;
                ui->proBar->setValue((recSize/fileSize) *100);
            }

            //如果接收大小和文件大小相同表示接收完毕
            if(recSize == fileSize)
            {
                file.close();
                QMessageBox::about(this,"提示","文件接收完毕.");
                ui->recMsg->append(QString("%1:文件接收完毕").arg(filename));
            }

    });



}

Clinet::~Clinet()
{
    delete ui;
}

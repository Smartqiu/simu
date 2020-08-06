#include "form.h"
#include "ui_form.h"
#include <QRegExpValidator>
#include <QHostAddress>
#include <QNetworkInterface>

Form* CreateWidget(QWidget* widget)
{
    return Form::instance(widget);
}



Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    //分配空间，指定父对象

    //接受主程序发来的数据，用2000端口号监听，socket加入到了组播224.0.0.2中
    //创建socket
    udpSocket = new QUdpSocket(this);
    //组播的绑定
    udpSocket->bind(QHostAddress::AnyIPv4,2000);
    //加入组播
    udpSocket->joinMulticastGroup(QHostAddress("224.0.0.2"));
    //持续监听，发现命令后处理
    connect(udpSocket, &QUdpSocket::readyRead, this, &Form::dealMsg);


}


////////////////////////////////////////////接口函数////////////////////////
Form::~Form()
{
    delete ui;
}

//单例模式
Form* Form::s_instance = NULL;

Form* Form::instance(QWidget* widget)
{
    if (NULL == s_instance)
    {
        s_instance = new Form(widget);
        s_instance->hide();
    }
    return s_instance;
}

void Form::setSysCtrl(IFrame *framectrl)
{
    m_framectrl = framectrl;
}

QIcon* Form::GetModuleIcon()
{
    QIcon *icon = new QIcon(":/set2.png");
    return icon;
}

QString* Form::GetModuleText()
{
    return new QString("管理员控制");
}

//////////////////////////////////////////////////////////////////


//处理主程序发来的命令，命令内容是功能名称；
void Form::dealMsg()
{
    qDebug() << "jieshou";
    QByteArray ba;

    while(udpSocket->hasPendingDatagrams())
    {
        ba.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(ba.data(), ba.size());
        QString itinfo = ba.data();
        qDebug() << "接收了：" << itinfo;
        if(itinfo =="功能1")
        {
            fun1();
            break;
        }
        else
        {

            qDebug() << "nofun";
            break;
        }
    }

}



//发送模块的ip，模块中功能数，功能名称
void Form::on_toolButton_clicked()

{
    itinfo = Form::getIP()+"#3"+"#功能1#功能2#功能3";
    QString port = "21778";
    udpSocket->writeDatagram(itinfo.toUtf8(), QHostAddress("224.0.0.2"), port.toShort());
    qDebug() <<  " fasong ok!" <<itinfo ;
}

QString Form::getIP()  //获取ip地址
{

    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list)
    {
       if(address.protocol() == QAbstractSocket::IPv4Protocol)//我们使用IPv4地址
            return address.toString();
    }
       return 0;
}


//////////////////////////////////////////////////////////----功能----/////////////////////////////////////////////
void Form::fun1()
{
    qDebug() << "fun111111 ok！";

}


void Form::fun2()
{
    qDebug() << "fun2 ok！";

}







#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QToolButton>
#include <QPixmap>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <myqframe.h>
#include <QWidget>
#include <QMouseEvent>
#include <QFont>
#include <QPalette>
#include <qfile.h>
#include <qtextstream.h>
#include <QtDebug>
#include <QSettings>
#include <QMdiSubWindow>
#include <QToolBar>
#include <QBitmap>
#include <QPainter>
#include <QStyle>
#include <QDesktopWidget>
#include <qudpsocket.h>
#include <udpsocket.h>
#include <QObject>
#include <modulesinterface.h>
#include <QNetworkInterface>

#define VALUE_DIS 5



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//////////////////////////////////////////////////////////////////////////////
    //固定大小
    resize(1400,900);
    ui->centralWidget->setMinimumSize(1200,400);

    // 无边框  Qt::FramelessWindowHint|Qt::WindowMinimizeButtonHint|Qt::WindowTitleHint
    setWindowFlags( Qt::CustomizeWindowHint);
    //关闭窗口
    connect(ui->closebtn,&QPushButton::clicked,this,&MainWindow::close);
    //最小化
    connect(ui->minbtn,&QPushButton::clicked,this,&MainWindow::showMinimized);
    //最大化,再次点击恢复窗口大小
    int i=0;
    connect(ui->maxbtn,&QPushButton::clicked,this,[&](){
        if(i%2==0)
        {
             connect(ui->maxbtn,&QPushButton::clicked,this,&MainWindow::showMaximized);
             i++;
        }
        else
        {
            connect(ui->maxbtn,&QPushButton::clicked,this,&MainWindow::showNormal);
            i++;
        }
     });
///////////////////////////////////////////////////


    //初始显示第0页
    ui->stackedWidget->setCurrentIndex(0);
    //设置模拟器标题和字号
    QFont ft;
    ft.setPointSize(12);
    ui->label_5->setFont(ft);

////////////////////////////////////////////////////////////


    //模块加载配置对话框初始化
    mdlg = NULL;
    //读取配置文件，需要加载哪些模块，已经模块的存放路径都存储于该文件中
    LoadModulesFromCfg();
    //信号槽匿名函数：创建并显示模块加载配置对话框
    connect(ui->toolButton,&QToolButton::clicked,[=](){

        if(mdlg == NULL)//当模块加载配置对话框未创建时，创建并建立信号槽机制
        {
            mdlg = new ModulesManager(this);
            void(ModulesManager::*sig)(QString) = &ModulesManager::sigReLoadModule;
            void(MainWindow::*slot)(QString) = &MainWindow::slotReloadModule;
            connect(mdlg,sig,this,slot);//信号槽：当模块加载配置对话框发送重载信号时，主界面重载所有模块组件
            //信号槽：当主界面发送刷新列表信号时，模块加载配置对话框刷新模块列表
            connect(this,&MainWindow::sigRefreshTable,mdlg,&ModulesManager::slotRefressTableWidget);
        }
        mdlg->show();
    });

/////////////////////////////////////////////////////////////////////////////
    //创建socket
    udpSocket = new QUdpSocket(this);
    //组播的绑定
    udpSocket->bind(QHostAddress::AnyIPv4,21778);//,QUdpSocket::ShareAddress
    //加入组播
    udpSocket->joinMulticastGroup(QHostAddress("224.0.0.2"));
    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::dealMsg);

    //双击目标方法进行回调处理
    connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(showSelectedImage(QTreeWidgetItem*)));

}
//////////////////////////////MainWindow结束！！！！/////////////////////////////////////

QString MainWindow::getIP()  //获取本机ip地址
{

    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list)
    {
       if(address.protocol() == QAbstractSocket::IPv4Protocol)//我们使用IPv4地址
            return address.toString();
    }
       return 0;
}

//接受模块发来的状态消息，添加到tree中
void MainWindow::dealMsg()
{

    QByteArray ba;
    qDebug() << "dealmsg ok";
    ui->treeWidget->setHeaderLabels(QStringList() << "方法名称" << "状态");

    while(udpSocket->hasPendingDatagrams())
    {

        ba.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(ba.data(), ba.size());
        QString itinfo = ba.data();

        qDebug() << "接收了：" << ba.data();

        QStringList iteminfolist = itinfo.split("#");
        QTreeWidgetItem* ip = new QTreeWidgetItem(QStringList(iteminfolist[0]));
        ui->treeWidget->addTopLevelItem(ip);
        for(int i = 0; i < iteminfolist[1].toInt(); ++i)
        {
            QTreeWidgetItem* funandState = new QTreeWidgetItem(QStringList(iteminfolist[i + 2]));
            ip->addChild(funandState);

        }

    }

}

//得到鼠标当前的功能名称和父亲ip
void MainWindow::showSelectedImage(QTreeWidgetItem *item)//, int column
{
    QTreeWidgetItem *parent = item->parent();
    if(NULL==parent) //注意：最顶端项是没有父节点的
        return;

    //点击时，当前index为0；
    QString nowip = item->parent()->text(0);

    QString nowfuncmes = item->text(0);

    MainWindow::sendfunc(nowip,"2000",nowfuncmes);

}

//发送鼠标双击的函数到对应的ip去，目标模块回调功能；
void MainWindow::sendfunc(QString ip, QString port, QString content)
{
    QUdpSocket qus;
    ip = "224.0.0.2";
    qus.writeDatagram(content.toUtf8(), QHostAddress(ip), port.toShort());
    qDebug() << "成功" << content << ip;
}

//管理员按钮，暂时起测试作用，没用
void MainWindow::on_toolButton_5_clicked()
{

    //itinfo = MainWindow::getIP()+"#3"+"#功能1#功能2#功能3";
    QString n = "127.0.0.1";
    itinfo = n+"#3"+"#功能1#功能2#功能3";
    QString port = "21778";
    udpSocket->writeDatagram(itinfo.toUtf8(), QHostAddress("224.0.0.2"), port.toShort());

    n = "333.213.321.456";
    itinfo = n+"#3"+"#功能1#功能2#功能3";
    udpSocket->writeDatagram(itinfo.toUtf8(), QHostAddress("224.0.0.2"), port.toShort());

    qDebug() <<  " fasong ok!111" ;
}



//////////////////////////////////////////////////////////////////////////////////////////

//模块加载
void MainWindow::slotReloadModule(QString path)
{
    //加载模块
    LoadModule(path);
    //发送刷新信号
    emit sigRefreshTable();
}

//卸载模块
void MainWindow::UnloadModule(QString &path)
{
    if(m_mapModules.find(path) != m_mapModules.end())
    {
        QMap<QString,QWidget*>::iterator itr = m_mapModulesWidget.find(path);
        if(itr != m_mapModulesWidget.end())
        {
            itr.value()->setAttribute(Qt::WA_DeleteOnClose);
            itr.value()->close();

            m_mapModulesWidget.remove(path);

        }
        QMap<QString,QAction*>::iterator itr2 = m_mapModulesActions.find(path);
        if(itr2 != m_mapModulesActions.end())
        {
            ui->toolButton_5->removeAction(itr2.value());
            m_mapModulesActions.remove(path);

        }

        if(m_mapModules[path]->unload())
        {
            m_mapModules.remove(path);

        }

    }

}

//添加模块
int btnar = 1;
QString MainWindow::LoadModule(QString &path)
{
    QIcon* icon = NULL;
    QString* text = NULL;
    //若该模块已加载，则先卸载
    UnloadModule(path);

    //动态加载共享库模块
    QLibrary *lib = new QLibrary(path);
    if(lib->load())
    {
        //将该模块储于模块列表
        m_mapModules[path]= lib;

        // 解析符号
        typedef QWidget* (*CreateFunc)(QWidget*);
        CreateFunc createDog = (CreateFunc)lib->resolve("CreateWidget");

        int a1=btnar;
        QWidget *pa;
        switch (a1)
        {
        case 1:
            pa=ui->page;
            break;
        case 2:
            pa=ui->page_2;
            break;
        case 3:
            pa=ui->page_3;
            break;
        case 4:
            pa=ui->page_4;
            break;
        case 5:
            pa=ui->page_5;
            break;
        case 6:
            pa=ui->page_6;
            break;
        case 7:
            pa=ui->page_7;
            break;
        case 8:
            pa=ui->page_8;
            break;
        case 9:
            pa=ui->page_9;
            break;
        case 10:
            pa=ui->page_10;
            break;
        case 11:
            pa=ui->page_11;
            break;
        default:
            pa=ui->page_12;
            break;
        }

        if(createDog != NULL)
        {

            IModules* module = dynamic_cast<IModules*>(createDog(pa));

            if (module)
            {
                icon = module->GetModuleIcon();
                text = module->GetModuleText();
                UDPSocket *udp = new UDPSocket();
                module->setSysCtrl(udp);
            }
        }
    }
    else
    {
        qDebug() << lib->errorString();
        return "";
    }


    //添加启动按钮*icon,*text,this
    QToolButton *btn = new QToolButton(this);
    btn->setText(*text);
    btn->setIcon(*icon);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->resize(150,80);//按钮大小
    btn->setIconSize(QSize(100,50));

//  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(btnar<=6)
    {
        btn->move(btnar*150,80);//位置
    }
    else
    {
        btn->move((btnar-6)*200,160);//位置
    }
    btn->setStyleSheet("QToolButton{font-family:'幼圆';font-size:20px;color:rgb(115,45,0);}");
    btn->setAutoRaise(true);//透明


    //链接到每个page中。
    int a=btnar;
    ui->stackedWidget->setCurrentIndex(0);//指定index0为初始界面
    connect(btn,&QPushButton::clicked,[=](){
       ui->stackedWidget->setCurrentIndex(a-1);
    });

    btnar++;


    //信号槽：点击主界面启动按钮时，创建该模块的显示界面
    connect(btn,&QPushButton::clicked,[=](){
        //m_mapModulesWidget  该map用于存储已经创建了界面的“模块路径-模块显示界面”
        QMap<QString,QWidget*>::iterator itr = m_mapModulesWidget.find(path);//查找该模块显示界面是否被创建
        if(itr != m_mapModulesWidget.end())//若该模块已经被创建，则无需创建，直接显示
        {
            itr.value()->show();
        }
        else    //若该模块未创建，则需要创建该显示界面
        {
            if(m_mapModules[path]!=NULL)
            {
                QLibrary* lib = m_mapModules[path];//在模块共享库map中取出该共享库调用实例
                typedef QWidget* (*CreateFunc)(QWidget*);
                CreateFunc createDog = (CreateFunc)lib->resolve("CreateWidget");
                if (createDog) {//创建并记录该共享库的“模块路径-模块显示界面”
                    //pa指针指向page，以此将库内容显示到page上
                    QWidget *pa;
                    switch (a)
                    {
                    case 1:
                        pa=ui->page;
                        break;
                    case 2:
                        pa=ui->page_2;
                        break;
                    case 3:
                        pa=ui->page_3;
                        break;
                    case 4:
                        pa=ui->page_4;
                        break;
                    case 5:
                        pa=ui->page_5;
                        break;
                    case 6:
                        pa=ui->page_6;
                        break;
                    case 7:
                        pa=ui->page_7;
                        break;
                    case 8:
                        pa=ui->page_8;
                        break;
                    case 9:
                        pa=ui->page_9;
                        break;
                    case 10:
                        pa=ui->page_10;
                        break;
                    case 11:
                        pa=ui->page_11;
                        break;
                    default:
                        pa=ui->page_12;
                        break;
                    }
                    mWidget = createDog(pa);

                    mWidget->show();

                    m_mapModulesWidget[path]= mWidget;

                } else {//若未实现该接口，则卸载该共享库模块
                    lib->unload();
                }

            }
            else
            {
                qDebug() << lib->errorString();
            }
        }
    });
    //释放资源图片、文字资源
    if(icon != NULL)
    {
        delete icon;
    }
    if(text != NULL)
    {
        delete text;

    }

    //返回模块的名称
    return btn->text();
}




void MainWindow::LoadModulesFromCfg()
{
    //配置文件存放位置为运行目录跟目录下
    QString pathFile =  QCoreApplication::applicationDirPath() + "/Config.ini";
    QSettings *configIniRead = new QSettings(pathFile, QSettings::IniFormat);
    //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型
    configIniRead->setIniCodec("UTF8");
    int count = configIniRead->value("num/count").toInt();//读取num下count的值，表示总共需要加载的模块数量
    for (int i = 0; i < count ; i++) {
        //读取模块i共享库的存放路径
        QString str = QString("module%1/path").arg(i+1);
        QString modulePath = configIniRead->value(str).toString();
        //按照存放路径加载共享库
        QString moduleName = LoadModule(modulePath);

        //如果共享库是新添加的，记录该模块的模块名称
        if(moduleName!= "")
        {
            QSettings *configIniWrite = new QSettings(pathFile,QSettings::IniFormat);
            configIniWrite->setIniCodec("UTF8");
            QString str = QString("module%1/name").arg(i+1);
            configIniWrite->setValue(str,moduleName);
            if(configIniWrite != NULL)
                delete configIniWrite;
        }
    }

    //读入入完成后删除指针
    delete configIniRead;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

MainWindow::~MainWindow()
{
    delete ui;
}




////////////////////////////////////以下貌似没啥用//////////////////////////////////////////////
//设置背景图片自适应
void MainWindow::setBackgroundImage(QPixmap &pixmap)
{
    //判断图片是否为空
    if(pixmap.isNull()){
        qDebug() << tr("illege arguments") <<endl;
        return;
    }
    //设置窗口的背景
    QPalette    palette = this->palette();
    palette.setBrush(this->backgroundRole(),
                     QBrush(pixmap.scaled(this->size(),
                            Qt::IgnoreAspectRatio,
                            Qt::SmoothTransformation)));

    this->setPalette(palette);
}

//重载resizeEvent
void MainWindow::resizeEvent(QResizeEvent *)
{
    QString path = ":/beijing3.jpg";
    QPixmap pixmap = QPixmap(path).scaled(this->size());
    setBackgroundImage(pixmap);
}

//鼠标点击事件
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    //读取坐鼠标点击坐标点
    m_lastPoint = event->globalPos();
}

//鼠标移动事件
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    //把移动的点记录下来
    int dx = event->globalX() - m_lastPoint.x();
    int dy = event->globalY() - m_lastPoint.y();

    m_lastPoint = event->globalPos(); //更新记录点

    move(x() + dx, y() + dy); //窗口移动到此处
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    //记录移动到的坐标
    int dx = event->globalX() - m_lastPoint.x();
    int dy = event->globalY() - m_lastPoint.y();

    move(x() + dx, y() + dy);
}






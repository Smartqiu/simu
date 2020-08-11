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
#include <QTimer>
#include <QTime>
#include <vector>
#define VALUE_DIS 5

#define qout qDebug()

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
    connect(ui->maxbtn,&QPushButton::clicked,this,[&](){
        if(minandmax==0)
        {
            connect(ui->maxbtn,&QPushButton::clicked,this,&MainWindow::showMaximized);
            minandmax = 1;
        }
        else
        {
            connect(ui->maxbtn,&QPushButton::clicked,this,&MainWindow::showNormal);
            minandmax = 0;
        }
    });

    // 管理员按钮点击显示或隐藏状态信息
    ui->stackedWidget_2->setCurrentIndex(0);
    connect(ui->toolButton_5,&QToolButton::clicked,this,[&](){
        if(btn5 == 0)
        {
            connect(ui->toolButton_5,&QToolButton::clicked,[&](){
                ui->stackedWidget_2->setCurrentIndex(btn5);
            });
            btn5 = 1;
        }
        else
        {
            connect(ui->toolButton_5,&QPushButton::clicked,[&](){
                ui->stackedWidget_2->setCurrentIndex(btn5);
            });
            btn5 = 0;
        }
    });

//////////////////////////////////////////////////////////////////////


    //初始显示第0页
    ui->stackedWidget->setCurrentIndex(0);
    //设置模拟器标题和字号
    QFont ft;
    ft.setPointSize(12);
    ui->label_5->setFont(ft);

/////////////////////////////////////////////////////////////////////////


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

///////////////////////////////////////////////////////////////////////////////////
    //创建socket
    udpSocket = new QUdpSocket(this);
    //组播的绑定
    udpSocket->bind(QHostAddress::AnyIPv4,21778);//,QUdpSocket::ShareAddress
    //加入组播
    udpSocket->joinMulticastGroup(QHostAddress("224.0.0.2"));

    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::dealMsg);
    //每秒发送计时信号，各个报文消息生存时间减少
    startTimer(1000);

    //双击目标方法进行回调处理
//    connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(showSelectedImage(QTreeWidgetItem*)));

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

//每秒处理，超过5s删除报文以及tree中数据
void MainWindow::timerEvent(QTimerEvent *event)
{
    //当前报文和他的剩余生存时间
    QHash<QString,int>::iterator nowtime;
    for( nowtime=m_time.begin(); nowtime!=m_time.end(); ++nowtime)
    {
        nowtime.value()--;


        if(nowtime.value() == 0) //如果生存时间为0，则清除本条报文
        {
            int index = hadState.indexOf(nowtime.key());
            QStringList delmod = nowtime.key().split("#");

            //用 it 遍历treewidget
            QTreeWidgetItemIterator it(ui->treeWidget);
            while (*it)
            {
                if(delmod[0] == (*it)->text(0) ) //如果此时报文ip和当前指向的节点一样
                {
                    if(  (*it)->childCount() == 0 || (*it)->childCount() == 1 ) //剩余一个和0个子节点都要清除整个ip
                    {
                        int con = 0;
                        for(int n=0;n<ui->treeWidget->topLevelItemCount();n++)
                        {
                            if((*it) == ui->treeWidget->topLevelItem(n))
                            {
                                con = n;
                            }
                        }
                        ui->treeWidget->takeTopLevelItem(con);
                        hadState.remove(index);
                        m_time.remove(nowtime.key());
                        return;
                    }
                    else // 剩余多个子节点时只需删除一个
                    {
                        for(int modindex = 0; modindex < (*it)->childCount() ;modindex++)
                        {
                            if(delmod[1] == (*it)->child(modindex)->text(0) )
                            {
                                (*it)->removeChild((*it)->child(modindex));
                                hadState.remove(index);
                                m_time.remove(nowtime.key());
                                return;
                            }
                        }
                    }
                }
                ++it;
            }

            hadState.remove(index);
            m_time.remove(nowtime.key());
        }
    }
}


//接受模块发来的状态消息，添加到tree中
void MainWindow::dealMsg()
{
    QByteArray ba;
    ui->treeWidget->setHeaderLabels(QStringList() << "方法名称" << " "<< " "<< "状态");
    //设置状态栏间隔
    ui->treeWidget->setColumnWidth(0,175);
    ui->treeWidget->setColumnWidth(1,50);
    ui->treeWidget->setColumnWidth(2,50);
    ui->treeWidget->setColumnWidth(3,125);
    //默认展开
//    ui->treeWidget->expandAll();

    while(udpSocket->hasPendingDatagrams())
    {

        ba.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(ba.data(), ba.size());
        QString itinfo = ba.data();
        qDebug() << "接收了：" <<  itinfo;
        QStringList iteminfolist = itinfo.split("#");
        if(hadState.indexOf(itinfo) >= 0 )// 存在相同直接退出
        {
            m_time[itinfo] = 5;//生存时间重置
            return;
        }
        else//信息存在不同，分为ip不同，模块名不同，函数名不同
        {
            for(int i=0;i<hadState.count();i++)//判断ip是否存在相同
            {
                QStringList hadstateip = hadState[i].split("#");
                if(hadstateip[0] == iteminfolist[0])
                    ipsame = i;
                else
                    ipsame = -1;
            }

            if(ipsame >= 0)//ip相同
            {
                for(int i=0;i<hadState.count();i++)//判断ip是否存在相同
                {
                    QStringList olditem = hadState[i].split("#");
                    if(olditem[1] == iteminfolist[1])
                        ips = i;
                    else
                        ips = -1;
                }

                if(ips >= 0 )//模块名相同，证明功能不同，更新功能列表
                {
                    ui->treeWidget->clear();
                    hadState.remove(ips);
                    return;
                }
                else //模块名不同，新添加模块
                {
                    qDebug() << "添加新模块";
                    //添加模块、功能、功能所携带的按钮
                    QTreeWidgetItemIterator it(ui->treeWidget);
                    while (*it)
                    {
                        QString h = QString(iteminfolist[0]);
                        if((*it)->text(0) == h )
                        {
                            QTreeWidgetItem* mod = new QTreeWidgetItem(QStringList(iteminfolist[1]));
                            QTreeWidgetItem *nowipitem = *it;
                            nowipitem->addChild(mod);
                            for(int i = 1; i < iteminfolist[2].toInt()+1; ++i)
                            {
                                QTreeWidgetItem* funandState = new QTreeWidgetItem(QStringList(iteminfolist[i + 2]));
                                mod->addChild(funandState);

                                QPushButton *topLevelButton = new QPushButton(" 开启 ");
                                topLevelButton->resize(5,10);
                                QPushButton *topLevelButton2 = new QPushButton(" 关闭 ");
                                topLevelButton2->resize(5,5);

                                //设置 Item 内控件，0 是第几列
                                ui->treeWidget->setItemWidget(funandState, 1, topLevelButton);
                                ui->treeWidget->setItemWidget(funandState, 2, topLevelButton2);
                                //开启
                                connect(topLevelButton,&QPushButton::clicked,this,[=](){

                                    QPushButton *sendObj = qobject_cast<QPushButton*>(sender());
                                    QTreeWidgetItem *item = ui->treeWidget->itemAt(QPoint(sendObj->frameGeometry().x(),sendObj->frameGeometry().y()));

                                    QString nowip = item->parent()->parent()->text(0);

                                    QString nowfuncmes = item->text(0);

                                    MainWindow::sendfunc(nowip,"2000",nowfuncmes+"#1");
                                });
                                //关闭
                                connect(topLevelButton2,&QPushButton::clicked,this,[=](){

                                    QPushButton *sendObj = qobject_cast<QPushButton*>(sender());
                                    QTreeWidgetItem *item = ui->treeWidget->itemAt(QPoint(sendObj->frameGeometry().x(),sendObj->frameGeometry().y()));

                                    QString nowip = item->parent()->parent()->text(0);

                                    QString nowfuncmes = item->text(0);

                                    MainWindow::sendfunc(nowip,"2000",nowfuncmes+"#0");
                                });
                            }
                            hadState << itinfo;
                            m_time.insert(itinfo,5);
                            return;
                        }
                        ++it;
                    }
                    return;
                }
            }

            hadState << itinfo;
            m_time.insert(itinfo,5);
        }
        //添加一整个ip、模块、功能；
        QTreeWidgetItem* ip = new QTreeWidgetItem(QStringList(iteminfolist[0]));
        ui->treeWidget->addTopLevelItem(ip);

        QTreeWidgetItem* modl = new QTreeWidgetItem(QStringList(iteminfolist[1]));
        ip->addChild(modl);
        for(int i = 1; i < iteminfolist[2].toInt()+1; ++i)
        {
            QTreeWidgetItem* funandState = new QTreeWidgetItem(QStringList(iteminfolist[i + 2]));
            modl->addChild(funandState);
            QPushButton *topLevelButton = new QPushButton(" 开启 ");
            topLevelButton->resize(5,10);
            QPushButton *topLevelButton2 = new QPushButton(" 关闭 ");
            topLevelButton2->resize(5,5);
            //设置 Item 内控件，0 是第几列
            ui->treeWidget->setItemWidget(funandState, 1, topLevelButton);
            ui->treeWidget->setItemWidget(funandState, 2, topLevelButton2);
            //开启
            connect(topLevelButton,&QPushButton::clicked,this,[=](){
                QPushButton *sendObj = qobject_cast<QPushButton*>(sender());
                QTreeWidgetItem *item = ui->treeWidget->itemAt(QPoint(sendObj->frameGeometry().x(),sendObj->frameGeometry().y()));
                QString nowip = item->parent()->parent()->text(0);
                QString nowfuncmes = item->text(0);
                MainWindow::sendfunc(nowip,"2000",nowfuncmes+"#1");
            });
            //关闭
            connect(topLevelButton2,&QPushButton::clicked,this,[=](){
                QPushButton *sendObj = qobject_cast<QPushButton*>(sender());
                QTreeWidgetItem *item = ui->treeWidget->itemAt(QPoint(sendObj->frameGeometry().x(),sendObj->frameGeometry().y()));
                QString nowip = item->parent()->parent()->text(0);
                QString nowfuncmes = item->text(0);
                MainWindow::sendfunc(nowip,"2000",nowfuncmes+"#0");
            });
        }
    }
}



////得到鼠标当前的功能名称和父亲ip
//void MainWindow::showSelectedImage(QTreeWidgetItem *item)//, int column
//{
//    QTreeWidgetItem *parent = item->parent();
//    if(NULL==parent) //注意：最顶端项是没有父节点的
//        return;
//    //点击时，当前index为0；
//    QString nowip = item->parent()->text(0);
//    QString nowfuncmes = item->text(0);
//    MainWindow::sendfunc(nowip,"2000",nowfuncmes);

//}


//发送函数到对应的ip去，目标模块回调功能；
void MainWindow::sendfunc(QString ip, QString port, QString content)
{
    QUdpSocket qus;
    ip = "224.0.0.2";                  ////////////////////////////////////////////////////////这里的ip还有问题
    qus.writeDatagram(content.toUtf8(), QHostAddress(ip), port.toShort());
    qDebug() << "成功" << content ;
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




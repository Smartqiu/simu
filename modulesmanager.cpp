#include "modulesmanager.h"
#include "ui_modulesmanager.h"
#include <QSettings>
#include <QDebug>
#include <QFileDialog>
#include "mainwindow.h"

ModulesManager::ModulesManager(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModulesManager)
{
    ui->setupUi(this);
    //添加
    connect(ui->pushButton,&QPushButton::released,this,&ModulesManager::AddModule);
    //删除
    connect(ui->pushButton_2,&QPushButton::released,this,&ModulesManager::DeleteModule);
    //重载
    connect(ui->pushButton_3,&QPushButton::released,this,&ModulesManager::ReloadModule);
    qDebug()<<"初始化";
    InitTableWidget();

}

ModulesManager::~ModulesManager()
{
    delete ui;
}

void ModulesManager::AddModule()
{
    //选择共享库并添加
    QStringList fileNames = QFileDialog::getOpenFileNames(this,"文件选择","",tr("动态库(*.dll *.so *.dylib);;"));
    qDebug() << fileNames;
    int oldCount = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(oldCount+fileNames.size());
    for (int i = 0; i < fileNames.size(); i++) {
        QString str = fileNames[i];
        int istart = str.lastIndexOf("/");
        int iend = str.lastIndexOf(".");
        str = str.mid(istart+1,iend-istart-1);
        ui->tableWidget->setItem(oldCount+i,0,new QTableWidgetItem("未识别"));
        ui->tableWidget->setItem(oldCount+i,1,new QTableWidgetItem(str));
        ui->tableWidget->setItem(oldCount+i,2,new QTableWidgetItem(fileNames[i]));
    }

}

void ModulesManager::DeleteModule()
{
    //删除共享库
    if(ui->tableWidget->selectedItems().size() <= 0 ) return;
    int rownum = ui->tableWidget->selectedItems()[0]->row();
    ui->tableWidget->removeRow(rownum);
}

void ModulesManager::slotRefressTableWidget()
{
    InitTableWidget();
}

void ModulesManager::InitTableWidget()
{
    ui->tableWidget->clear();
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()<<"名称"<<"模块"<<"路径");

    QString pathFile =  QCoreApplication::applicationDirPath() + "/Config.ini";
    QSettings *configIniRead = new QSettings(pathFile, QSettings::IniFormat);
    //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型
    configIniRead->setIniCodec("UTF8");
    QStringList listNames(QStringList()<<"name"<<"module"<<"path");
    int count = configIniRead->value("num/count").toInt();
    ui->tableWidget->setRowCount(count);
    for (int i = 0; i < count ; i++) {
        for (int j = 0; j < 3; j++) {

            QString str = QString("module%1/%2").arg(i+1).arg(listNames[j]);
            ui->tableWidget->setItem(i,j,new QTableWidgetItem(configIniRead->value(str).toString()));
        }
    }

    //读入入完成后删除指针
    delete configIniRead;

}

void ModulesManager::ReloadModule()
{
    if(ui->tableWidget->selectedItems().size() <= 0 ) return;
    int rownum = ui->tableWidget->selectedItems()[0]->row();
    QString path = ui->tableWidget->item(rownum,2)->text();
    qDebug() << path;
    emit sigReLoadModule(path);

}

void ModulesManager::closeEvent(QCloseEvent *e)
{
    QString pathFile =  QCoreApplication::applicationDirPath() + "/Config.ini";
    QSettings *configIniWrite = new QSettings(pathFile,QSettings::IniFormat);

    int count = ui->tableWidget->rowCount();
    configIniWrite->setIniCodec("UTF8");
    QStringList listNames(QStringList()<<"name"<<"module"<<"path");
    for (int i = 0; i < count; i++) {
        for(int j = 0; j < 3; j++){
        //向ini文件的第一个节写入内容,ip节下的第一个参数
        QString str = QString("module%1/%2").arg(i+1).arg(listNames[j]);
        QString str2 = ui->tableWidget->item(i,j)->text();
        configIniWrite->setValue(str,str2);
        }
    }
    configIniWrite->setValue("num/count",count);
    //写入完成后删除指针
    delete configIniWrite;
    QDialog::closeEvent(e);
}

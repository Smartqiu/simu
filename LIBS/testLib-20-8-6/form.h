#ifndef FORM_H
#define FORM_H

#include <QDialog>
#include "testLib_global.h"
#include "modulesinterface.h"
#include <QWidget>
#include <QUdpSocket>
#include <QTreeWidget>

namespace Ui {
class Form;
}
//主框架接口框架
#define afs s_instance->GetSysCtrl()

class Form : public QWidget,public IModules
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();
    static Form* instance(QWidget*);    //单例模式
    virtual QIcon *GetModuleIcon() override;
    virtual QString *GetModuleText()override;
    void setSysCtrl(IFrame *framectrl) override;
    IFrame* GetSysCtrl(){return m_framectrl;}
    static void ResolveData(QByteArray&);       //解析数据
    QString getIP();
    void dealMsg();//槽函数，处理对方发过来的数据（槽函数的形式与信号保持一致，没有参数）

    void fun1();
    void fun2();

private slots:
    //void on_pushButton_released();
    //void on_pushButton_2_released();
    void on_toolButton_clicked();
    void showSelectedImage(QTreeWidgetItem * item, int column); //点击树节点事件

private:
    Ui::Form *ui;
    static Form* s_instance;
    QUdpSocket *udpSocket;//UDP套接字
    QString itinfo;

};
//对外输出的函数
extern "C" Q_DECL_EXPORT Form* CreateWidget(QWidget*);

#endif // FORM_H

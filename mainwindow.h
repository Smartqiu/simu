#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mainwindow.h"
#include <QPoint>
#include "modulesmanager.h"
#include <QHash>
#include <QLibrary>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QPushButton>
#include <QUdpSocket>
#include <udpsocket.h>
#include "csysctrl.h"
#include <QTreeWidget>
#include <QVector>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class  Form;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void on_newsend_clicked();
    QString getIP();
    void dealMsg();//槽函数，处理对方发过来的数据（槽函数的形式与信号保持一致，没有参数）

    void sendfunc(QString ip, QString port, QString content);

    QVector< QString >hadState;

    int ipsame = -1; //每次到来的ip是否相同
    int ips = -1;
    int minandmax=0;
    int btn5 = 1;

    QHash<QString,int> m_time;
    QTimer *time;
    virtual void timerEvent(QTimerEvent *event); //定时器

signals:
    void sigRefreshTable();
public slots:
    void slotReloadModule(QString path);



private:
    void LoadQSS();
    QString LoadModule(QString& path);
    void LoadModulesFromCfg();

    void UnloadModule(QString &path);

    void setBackgroundImage(QPixmap &pixmap);
    void resizeEvent(QResizeEvent *);



private:
    Ui::MainWindow *ui;
    //退出按钮
    QPushButton *m_buttonClose;

    //模块lib加载
    CSysCtrl* m_sysctrl;
    QLibrary m_dll;
    ModulesManager *mdlg ;
    QMap<QString,QLibrary*> m_mapModules;
    QMap<QString,QWidget*> m_mapModulesWidget;
    QMap<QString,QAction*> m_mapModulesActions;
    bool m_bLeftBtnPress;

    //鼠标
    QPoint m_lastPoint;
    QPoint m_ptPressGlobal;

    //socket对象
    QUdpSocket *uSocket;
    QUdpSocket *udpSocket;
    QUdpSocket qus;

    QString itinfo;
    QWidget *mWidget;

public slots:

private slots:



protected:

    //鼠标点击事件
    void mousePressEvent(QMouseEvent *event);
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event);
    //鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event);

};

#endif // MAINWINDOW_H

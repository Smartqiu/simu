#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mainwindow.h"
#include <QPoint>
#include "modulesmanager.h"
#include <QMap>
#include <QLibrary>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QPushButton>
#include <QUdpSocket>
#include <udpsocket.h>
#include "csysctrl.h"
#include <QTreeWidget>

#define cout qDebug() << "[" << __FILE__ << ":" << __LINE__ << "]"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
//方向枚举
enum enum_Direction
{
   eNone,
   eTop = 1,
   eRight =2 ,
   eBottom =4,
   eLeft = 8,
   eTopRight = eTop + eRight,
   eRightBottom = eRight + eBottom,
   eBottomLeft = eBottom + eLeft,
   eLeftTop = eLeft + eTop
};
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

signals:
    void sigRefreshTable();
public slots:
    void slotReloadModule(QString path);

    void showSelectedImage(QTreeWidgetItem * item); //点击树节点事件, int column

private:
    void LoadQSS();
    QString LoadModule(QString& path);
    void LoadModulesFromCfg();

    void SetCursorStyle(enum_Direction direction);

    enum_Direction PointValid(int nXRelative,int nYRelative);
    void SetDrayMove(int nXGlobal,int nYGlobal,enum_Direction direction);
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
    enum_Direction m_eDirection;
    QWidget *mWidget;

public slots:
    //void receive();
private slots:
    void on_toolButton_5_clicked();


protected:

    //鼠标点击事件
    void mousePressEvent(QMouseEvent *event);
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event);
    //鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event);

};

#endif // MAINWINDOW_H

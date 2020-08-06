#include "myqframe.h"
#include <QDebug>

MyQFrame::MyQFrame()
{
    this->setMouseTracking(true);// 此属性没有设为true则不会监听到mouseMove
}
MyQFrame::~MyQFrame()
{

}
void MyQFrame::mousePressEvent(QMouseEvent *e)
{
    // 此事件只在此地触发，没必要让父窗口捕捉到，因此也就不需要抛出QFrame::mousePressEvent
    myQFramePress(e);
}
void MyQFrame::mouseMoveEvent(QMouseEvent *e)
{
    // 注意：在此抛出QFrame::mouseMoveEvent(e);则在父窗口可以接收到mouseMoveEvent事件，否则接收不到
    // 另外，必须设置this->setMouseTracking(true);否则不能触发此回调函数
    QFrame::mouseMoveEvent(e);
}
void MyQFrame::mouseReleaseEvent(QMouseEvent *e)
{
    myQFrameRelease(e);
}
void MyQFrame::mouseDoubleClickEvent(QMouseEvent *e)
{
//    qDebug()<<"doubleclick!";
}

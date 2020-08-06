#ifndef MYQFRAME_H
#define MYQFRAME_H

#include <QFrame>


class MyQFrame : public QFrame
{
    Q_OBJECT
public:
    explicit MyQFrame();
    ~MyQFrame();
protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);

signals:
    void myQFramePress(QMouseEvent *e);// 注意，信号只需注册，并在cpp里调用，无需实现
    void myQFrameRelease(QMouseEvent *e);
};

#endif

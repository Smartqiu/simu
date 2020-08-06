#ifndef MYUDP1_H
#define MYUDP1_H

#include <QWidget>

namespace Ui {
class myudp1;
}

class myudp1 : public QWidget
{
    Q_OBJECT

public:
    explicit myudp1(QWidget *parent = 0);
    ~myudp1();

private:
    Ui::myudp1 *ui;
};

#endif // MYUDP1_H

#include "myudp1.h"
#include "ui_myudp1.h"

myudp1::myudp1(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::myudp1)
{
    ui->setupUi(this);
}

myudp1::~myudp1()
{
    delete ui;
}

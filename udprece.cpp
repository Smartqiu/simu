#include "udprece.h"
#include "ui_udprece.h"
#include <QSettings>
#include <QDebug>
#include <QFileDialog>
#include "mainwindow.h"

UDPrece::UDPrece(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UDPrece)
{
    ui->setupUi(this);
    ui->textEdit->setText("21778");
    uSocket = new QUdpSocket;
    uSocket->bind(21778,QUdpSocket::ShareAddress);
    connect(uSocket, SIGNAL(readyRead()), this, SLOT(receive()));
}

UDPrece::~UDPrece()
{
    delete ui;
}

void UDPrece::receive()
{
    QByteArray ba;
    while(uSocket->hasPendingDatagrams())
    {
        ba.resize(uSocket->pendingDatagramSize());
        uSocket->readDatagram(ba.data(), ba.size());
        ui->textEdit_2->setPlainText(ba);
    }
}

void UDPrece::on_pushButton_released()
{
    if(uSocket != NULL)
    {
        uSocket->abort();
        QString port = ui->textEdit->toPlainText();
        uSocket->bind(port.toShort(),QUdpSocket::ShareAddress);
    }

}

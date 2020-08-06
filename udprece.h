#ifndef UDPRECE_H
#define UDPRECE_H

#include <QDialog>
#include <QUdpSocket>
#include <mainwindow.h>
#include <ui_udprece.h>

namespace Ui {
class UDPrece;
}

class UDPrece : public QDialog
{
    Q_OBJECT

public:
    explicit UDPrece(QWidget *parent = 0);
    ~UDPrece();

public slots:
    void receive();
private slots:
    void on_pushButton_released();

private:
    Ui::UDPrece *ui;
    QUdpSocket *uSocket;
};

#endif // UDPRECE_H

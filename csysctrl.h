#ifndef CSYSCTRL_H
#define CSYSCTRL_H

#include <QObject>
#include "declare.h"

class AFS_SHARED_IMPORT CSysCtrl : public QObject
{
    Q_OBJECT
public:

signals:

public slots:
    virtual void CreateRecv() = 0;
    virtual void sendMsg(QString,QString,QString) = 0;
    virtual void sendMulticastMsg(QString, QString, QString) = 0;
protected:
    CSysCtrl(){}
    virtual ~CSysCtrl(){}
};

#endif // CSYSCTRL_H
/////////////////////////////////////////////////////////////////////////

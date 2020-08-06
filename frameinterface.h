#ifndef FRAMEINTERFACE_H
#define FRAMEINTERFACE_H
#include <QObject>
#include "declare.h"

//主框架提供的接口AFS_SHARED_IMPORT
class  AFS_SHARED_IMPORT IFrame : public QObject
{
public:
    virtual void sendMsg(QString,QString,QString) = 0;//发送接口

    virtual void createReceiver(QString,void(*func)(QByteArray&)) = 0;//接收接口

    virtual void sendMulticastMsg(QString, QString, QString) = 0;
};
#endif // FRAMEINTERFACE_H

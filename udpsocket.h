#ifndef UDPSOCKET_H
#define UDPSOCKET_H
#include "frameinterface.h"
#include <QHash>
#include <QSet>
#include <QUdpSocket>
#include <QHostAddress>
#include <QFunctionPointer>

typedef void(*myfunction)(QByteArray&);

class UDPSocket : public IFrame
{
public:
    UDPSocket();
    void sendMsg(QString, QString, QString) override;
    void sendMulticastMsg(QString, QString, QString) override;
    void createReceiver(QString, void(*func)(QByteArray&)) override;
private:
    static QHash<QString, QVector<myfunction>> m_funcset;
    QUdpSocket* m_receiver;
    QHostAddress m_castaddr{"224.0.0.2"};
};

#endif // UDPSOCKET_H

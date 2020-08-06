#include "udpsocket.h"
#include <mainwindow.h>
#include <ui_mainwindow.h>

QHash<QString, QVector<myfunction>> UDPSocket::m_funcset = {};

void UDPSocket::sendMsg(QString ip, QString port, QString content)
{
    QUdpSocket udpsocekt;
    udpsocekt.writeDatagram(content.toUtf8(), QHostAddress(ip), port.toShort());    // 向指定ip发送数据
}



void UDPSocket::createReceiver(QString port, void(*func)(QByteArray& tmpBa))
{
    QHash<QString, QVector<myfunction>>::iterator funcs = m_funcset.find(port);
    if(funcs != m_funcset.end())
    {
        if(!((funcs.value()).contains(func)))
            (funcs.value()).push_back(func);
    }
    else
    {
        m_funcset[port].push_back(func);
        m_receiver = new QUdpSocket;
        m_receiver->bind(QHostAddress::AnyIPv4, port.toShort(), QUdpSocket::ShareAddress);
        m_receiver->joinMulticastGroup(m_castaddr);    // 加入某个组播
        m_receiver->setSocketOption(QAbstractSocket::MulticastTtlOption, 18);    // 设置TTL
        connect(m_receiver, &QIODevice::readyRead, this, [=]() {
            QByteArray ba;
            while(m_receiver->hasPendingDatagrams())
            {
                ba.resize(m_receiver->pendingDatagramSize());
                m_receiver->readDatagram(ba.data(), ba.size());
                for(int i = 0; i < m_funcset[port].size(); ++i)
                {
                    (*(m_funcset[port][i]))(ba);
                }
            }
        });
    }
}

void UDPSocket::sendMulticastMsg(QString ip, QString port, QString content)
{
    QUdpSocket multicast;
    multicast.writeDatagram(content.toUtf8(), m_castaddr, port.toShort());    // 向组发送数据（组播）
}

UDPSocket::UDPSocket()
{

}

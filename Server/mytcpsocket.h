#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include "msghandler.h"
#include "protocol.h"

#include <QObject>
#include <QTcpSocket>

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    ~MyTcpSocket();
    //保存该客户端使用的用户名
    QString strUserName;
public slots:

    //处理消息
    PDU* handleMsg(PDU* pdu);
    //向客户端发送PDU单元
    void sendPDU(PDU* respdu);

    //接收并处理客户端发送的消息
    void recvMSG();
    //处理客户端断开连接
    void dicClient(); 
signals:
    //信号(客户端已断开连接)
    void dicedClient(MyTcpSocket* socket);
private:
    MsgHandler* handler;
    //数据缓冲区(避免粘包半包问题)
    QByteArray Buffer;
};

#endif // MYTCPSOCKET_H

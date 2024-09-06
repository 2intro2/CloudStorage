#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include "mytcpsocket.h"

#include <QObject>
#include <QTcpServer>
#include <QList>

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    //单例模式: 全局访问节点
    static MyTcpServer& getInstance();

    //重写incomingConnection函数用于管理客户端连接
    void incomingConnection(qintptr handle);
    //转发函数(像指定客户端发送pdu)
    void resend(const char* TarName,PDU* pdu);
public slots:
    //从列表中移除客户端(已断开连接)使用的socket
    void deleteClientSocket(MyTcpSocket* socket);
private:

    //创建列表用于保存客户端进行连接的套接字
    QList<MyTcpSocket*> Client_Socket_List;

    //单例模式
    MyTcpServer();
    MyTcpServer(const MyTcpServer& other) = delete;
    MyTcpServer& operator=(const MyTcpServer other) = delete;



};

#endif // MYTCPSERVER_H

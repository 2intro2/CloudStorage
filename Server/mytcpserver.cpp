#include "mytcpserver.h"

#include <QDebug>

MyTcpServer& MyTcpServer::getInstance()
{
    static MyTcpServer Tcp_Server;
    return Tcp_Server;
}

void MyTcpServer::incomingConnection(qintptr handle)
{
    qDebug()<<"新客户端连接成功";

    //将客户端进行连接的socket保存到Client_Socket_List中
    MyTcpSocket*Tcp_socket=new MyTcpSocket;
    Tcp_socket->setSocketDescriptor(handle); //使用句柄获取客户端进行连接的套接字
    Client_Socket_List.append(Tcp_socket);
    connect(Tcp_socket,&MyTcpSocket::dicedClient,this,&MyTcpServer::deleteClientSocket);
}

void MyTcpServer::resend(const char* TarName, PDU *pdu)
{
    if(pdu==NULL||TarName==NULL) return;
    //找到目标客户端
    for(int i=0;i<Client_Socket_List.size();i++)
    {
        if(Client_Socket_List[i]->strUserName==QString(TarName))
        {
            Client_Socket_List[i]->write((char*)pdu,pdu->uiPDULen);
            return;
        }
    }
}

void MyTcpServer::deleteClientSocket(MyTcpSocket *socket)
{
    Client_Socket_List.removeOne(socket); //移除操作
    socket->deleteLater(); //延迟释放
    socket=NULL;

    //Debug
    qDebug()<<"剩余连接客户端数量:"<<Client_Socket_List.size();
    for(int i=0;i<Client_Socket_List.size();i++)
    {
        qDebug()<<"客户端"<<i+1<<"使用的用户名"<<Client_Socket_List[i]->strUserName<<endl;
    }
}

MyTcpServer::MyTcpServer()
{

}

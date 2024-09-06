#ifndef CLIENT_H
#define CLIENT_H

#include "msghandler.h"
#include "protocol.h"

#include <QTcpSocket>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QWidget
{
    Q_OBJECT
public:
    //单例模式: 全局访问节点
    static Client& getInstance();

    //访问私有成员变量的节点
    //获取用于连接服务器的socket
    QTcpSocket& getTcpSocket();
    QString getUserName();

    ~Client();

    //向服务器发送PDU单元
    void sendPDU(PDU* pdu);
    //处理消息
    void handleMsg(PDU* pdu);

    //刷新好友列表
    void FlushFriendList();
public slots:
    void showConnect();

    //接收服务器发送的消息
    void recvMSG();

private slots:
    //客户端请求登录(点击登录按钮)
    void on_btn_login_clicked();
    //客户端请求注册(点击注册按钮)
    void on_btn_regist_clicked();

private:
    /*
     * 单例模式:单例模式是一种创建型设计模式,确保一个类只有一个实例存在,并提供一个全局访问点来访问这个实例
     * 将类的构造函数设置为私有 可以防止其他类直接获取该类的实例 只能通过全局访问节点获取该类的唯一实例
     * 同时删除拷贝构造函数和拷贝赋值运算符
    */
    Client(QWidget *parent = nullptr);
    Client(const Client& instance) = delete;
    Client& operator=(const Client& instance)=delete;

    //服务端IP 端口Port
    QString Server_IP;
    quint16 Server_Port;
    //用于连接服务器的socket
    QTcpSocket Tcp_Socket;
    //保存用户名
    QString strUserName;

    MsgHandler* handler;

    //数据缓冲区(避免粘包半包问题)
    QByteArray Buffer;

    //加载配置文件
    void loadConfig();

    Ui::Client *ui;
};
#endif // CLIENT_H

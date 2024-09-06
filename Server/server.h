#ifndef SERVER_H
#define SERVER_H

#include "mytcpserver.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class server; }
QT_END_NAMESPACE

class server : public QWidget
{
    Q_OBJECT

public:

    //加载配置文件
    void loadConfig();
    ~server();

    //获取文件系统根目录
    QString getRootPath();

    //全局访问节点
    static server& getInstance();
private:
    Ui::server *ui;

    //单例模式
    server(QWidget *parent = nullptr);
    server(const server& other) = delete;
    server& operator=(const server& other) =delete;

    //服务器IP和端口号
    QString Server_IP;
    quint16 Server_Port;
    //文件系统根目录
    QString RootPath;
    //创建服务器
    MyTcpServer& Tcp_Server;
};
#endif // SERVER_H

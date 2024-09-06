#include "server.h"
#include "ui_server.h"

#include <QFile>
#include <QDebug>

server::server(QWidget *parent): QWidget(parent), ui(new Ui::server),Tcp_Server(MyTcpServer::getInstance())
{
    ui->setupUi(this);
    //加载配置文件
    loadConfig();

    //监听是否有客户端进行连接(存在连接时服务器会自动调用incomingConnection函数管理连接)
    Tcp_Server.listen(QHostAddress(Server_IP),Server_Port);


}

void server::loadConfig()
{
    QFile server_config(":/server.config"); //创建文件对象操作文件
    if(server_config.open(QIODevice::ReadOnly)) //使用文件对象以只读方式打开问价
    {
        QString content=QString(server_config.readAll()); //以Qstring类型获取文件内容(将QByteArray强制转换为QString)
        QStringList config_list=content.split("\r\n"); //分割文件内容 并使用QStringList获取配置信息

        Server_IP=config_list[0];
        Server_Port=config_list[1].toUShort();
        RootPath=config_list[2];

        qDebug()<<"IP:"<<Server_IP<<"Port:"<<Server_Port;
        server_config.close(); //关闭文件
    }
    else qDebug()<<"配置文件打开失败!";
}

server::~server()
{
    delete ui;
}

QString server::getRootPath()
{
    return this->RootPath;
}

server &server::getInstance()
{
    static server Instance;
    return Instance;
}


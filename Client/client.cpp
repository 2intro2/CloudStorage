#include "client.h"
#include "protocol.h"
#include "ui_client.h"
#include "index.h"

#include <QFile>
#include <QHostAddress>
#include <QtDebug>
#include <QMessageBox>

//实现一个功能的总体流程: 客户端->服务器->数据库  数据库->服务器->客户端
Client::Client(QWidget *parent) : QWidget(parent) , ui(new Ui::Client)
{
    ui->setupUi(this);
    //加载qss文件美化登录界面
    QFile file(":/res/qss/style-1.qss");
    file.open(QFile::ReadOnly);
    QTextStream filetext(&file);
    QString stylesheet=filetext.readAll();
    this->setStyleSheet(stylesheet);
    file.close();

    this->resize(1280,960);
    this->setWindowIcon(QIcon(":/res/pic/yun.png"));
    this->setWindowTitle(QString("Qt网盘"));

    loadConfig();

    handler=new MsgHandler();

    //关联信号和槽函数(判断连接服务器是否成功)
    QWidget::connect(&Tcp_Socket,&QTcpSocket::connected,this,&Client::showConnect);
    //关联信号和槽(客户端一旦收到消息就开始接收)
    connect(&Tcp_Socket,&QTcpSocket::readyRead,this,&Client::recvMSG);
    //连接服务器
    Tcp_Socket.connectToHost(QHostAddress(Server_IP),Server_Port);
}

Client::~Client()
{
    delete ui;
    delete handler;
}

Client& Client::getInstance()
{
    //静态变量只能被初始化一次
    static Client instace;
    return instace;
}

QTcpSocket& Client::getTcpSocket()
{
    return Tcp_Socket;
}

QString Client::getUserName()
{
    return this->strUserName;
}

void Client::showConnect()
{
    qDebug()<<"连接服务器成功!";
}

void Client::sendPDU(PDU *pdu)
{
    Tcp_Socket.write((char*)pdu,pdu->uiPDULen);

    //发送消息后释放空间
    free(pdu);
    pdu=NULL;
}

void Client::handleMsg(PDU *pdu)
{

    qDebug()<<"协议数据单元大小(字节):"<<pdu->uiPDULen<<endl
            <<"实际消息长度:"<<pdu->uiMSgLen<<endl
            <<"消息类型:"<<pdu->uiMSGType<<endl
            <<"参数:"<<pdu->Parameter<<endl
            <<"消息:"<<pdu->MSG<<endl;

    //依据消息类型 选择处理消息的方法
    switch (pdu->uiMSGType)
    {
        //服务器返回注册请求的结果
        case ENUM_MSG_TYPE_REGIST_RESPOND:
        {
            handler->FC_REGIST(pdu);
            break;
        }
        //服务器返回登录请求的结果
        case ENUM_MSG_TYPE_LOGIN_RESPOND:
        {
            handler->FC_LOGIN(pdu);
            break;
        }
        //好友界面-返回查找用户请求
        case ENUM_MSG_TYPE_FINDUSER_RESPOND:
        {
            handler->FC_FINDUSER(pdu);
            break;
        }
        //好友界面-返回发送好友邀请请求
        case ENUM_MSG_TYPE_SENDINVATION_RESPOND:
        {
            handler->FC_SENDINVATION(pdu);
            break;
        }
        //好友界面-处理好友邀请请求
        case ENUM_MSG_TYPE_SENDINVATION_REQUEST:
        {
            handler->FC_HANDLEINVATION(pdu);
            break;
        }
        //好友界面-处理接受好友请求
        case ENUM_MSG_TYPE_ACCEPTINVATION_RESPOND:
        {
            handler->FC_ACCEPTINVATION(pdu);
            break;
        }
        //好友界面-返回在线用户请求
        case ENUM_MSG_TYPE_ONLINEUSERS_RESPOND:
        {
            handler->FC_ONLINEUSERS(pdu,&strUserName);
            break;
        }
        //服务器返回刷新好友列表的请求
        case ENUM_MSG_TYPE_FLUSHFRIENDLIST_RESPOND:
        {
            handler->FC_FLUSHFRIENDLIST(pdu);
            break;
        }
        //好友界面-返回删除用户请求
        case ENUM_MSG_TYPE_DELETEFRIEND_RESPOND:
        {
            handler->FC_DELETEFRIEND(pdu);
            break;
        }
        //接收好友发送的聊天消息
        case ENUM_MSG_TYPE_CHAT_RESPOND:
        {
            handler->FC_CHAT(pdu);
            break;
        }
        //文件界面-创建文件夹请求
        case ENUM_MSG_TYPE_MKDIR_RESPOND:
        {
            handler->FC_MKDIR(pdu);
            break;
        }
        //文件界面-刷新文件请求
        case ENUM_MSG_TYPE_FLUSHFILE_RESPOND:
        {
            handler->FC_FLUSHFILE(pdu);
            break;
        }
        //文件界面-删除文件夹请求
        case ENUM_MSG_TYPE_DELDIR_RESPOND:
        {
            handler->FC_DELDIR(pdu);
            break;
        }
        //文件界面-删除文件请求
        case ENUM_MSG_TYPE_DELFILE_RESPOND:
        {
            handler->FC_DELFILE(pdu);
            break;
        }
        //文件界面-重命名文件请求
        case ENUM_MSG_TYPE_RENAME_RESPOND:
        {
            handler->FC_RENAME(pdu);
            break;
        }
        //文件界面-移动文件请求
        case ENUM_MSG_TYPE_MOVEFILE_RESPOND:
        {
            handler->FC_MOVEFILE(pdu);
            break;
        }
        //文件界面-上传文件请求
        case ENUM_MSG_TYPE_UPLOADFILE_RESPOND:
        {
            handler->FC_UPLOADFILE(pdu);
            break;
        }
        //文件界面-上传文件数据请求
        case ENUM_MSG_TYPE_UPLOADFILEDATA_RESPOND:
        {
            handler->FC_UPLOADFILEDATA(pdu);
            break;
        }
        //文件界面-下载文件请求
        case ENUM_MSG_TYPE_DOWNLOADFILE_RESPOND:
        {
            handler->FC_DOWNLOADFILE(pdu);
            break;
        }
        //文件界面-下载文件数据请求
        case ENUM_MSG_TYPE_DOWNLOADFILEDATA_RESPOND:
        {
            handler->FC_DOWNLOADFILEDATA(pdu);
            break;
        }
        //文件界面-分享文件请求
        case ENUM_MSG_TYPE_SHAREFILE_RESPOND:
        {
            handler->FC_SHAREFILE(pdu);
            break;
        }
        //文件界面-分享文件请求(接收方)
        case ENUM_MSG_TYPE_SHAREFILE_REQUEST:
        {
            handler->FC_SHAREFILEREC(pdu);
            break;
        }
        //文件界面-分享文件请求(接收方-反馈)
        case ENUM_MSG_TYPE_SHAREFILEAGREE_RESPOND:
        {
            handler->FC_SHAREFILERECRES(pdu);
            break;
        }
        default: break;
    }
}

void Client::recvMSG()
{

    //设置接收数据的缓冲区 处理粘包和半包的问题

    //接收此次客户端发送的全部数据
    QByteArray CurData=Tcp_Socket.readAll();
    //将上一次未处理的数据和这一次接收的数据合并
    Buffer.append(CurData);

    //循环处理粘包数据  注意每一次处理Buffer BUffer的开头必为pdu的头部
    while(Buffer.size()>=(int)sizeof(PDU)) //通过是否含有PDU的头部来初步判断Buffer的数据是否仍为半包
    {
        //一次读取一个协议数据单元
        PDU* pdu=(PDU*) Buffer.data();
        //通过判断PDU中的数据是否完整来判断Buffer的数据是否为半包
        if(Buffer.size()<(int)pdu->uiPDULen) break;

        handleMsg(pdu);

        //释放缓冲区(清除以处理的数据)
        Buffer.remove(0,pdu->uiPDULen);
    }
}

void Client::loadConfig()
{
    QFile client_config(":/client.config"); //创建文件对象操作文件
    if(client_config.open(QIODevice::ReadOnly)) //使用文件对象以只读方式打开问价
    {
        QString content=QString(client_config.readAll()); //以Qstring类型获取文件内容(将QByteArray强制转换为QString)
        QStringList config_list=content.split("\r\n"); //分割文件内容 并使用QStringList获取配置信息

        Server_IP=config_list[0];
        Server_Port=config_list[1].toUShort();

        qDebug()<<"IP:"<<Server_IP<<"Port:"<<Server_Port;
        client_config.close(); //关闭文件
    }
    else qDebug()<<"配置文件打开失败!";
}

void Client::on_btn_login_clicked()
{
    //获取用户名和密码
    QString strName=ui->username->text();
    QString strPWD=ui->password->text();

    //判断用户名和密码是否合法
    if(strName.isEmpty()||strPWD.isEmpty())
    {
        QMessageBox::information(this,"Qt网盘","用户名或密码不能为空");
        return;
    }
    if(strName.toStdString().size()>32||strPWD.toStdString().size()>32)
    {
        QMessageBox::information(this,"Qt网盘","用户名或密码长度不合法");
        return;
    }
    //记录用户登录所使用的用户名
    strUserName=strName;

    //创建协议数据单元并发送至服务器
    PDU*pdu=mkPDU(0);
    pdu->uiMSGType=ENUM_MSG_TYPE_LOGIN_REQUSET;
    memcpy(pdu->Parameter,strName.toStdString().c_str(),32);
    memcpy(pdu->Parameter+32,strPWD.toStdString().c_str(),32);

    sendPDU(pdu);
}

void Client::on_btn_regist_clicked()
{
    //获取用户名和密码
    QString strName=ui->username->text();
    QString strPWD=ui->password->text();

    //判断用户名和密码是否合法
    if(strName.isEmpty()||strPWD.isEmpty())
    {
        QMessageBox::information(this,"Client","用户名或密码不能为空");
        return;
    }
    if(strName.toStdString().size()>32||strPWD.toStdString().size()>32)
    {
        QMessageBox::information(this,"Client","用户名或密码长度不合法");
        return;
    }

    strUserName=strName;
    //创建协议数据单元并发送至服务器
    PDU*pdu=mkPDU(0);
    pdu->uiMSGType=ENUM_MSG_TYPE_REGIST_REQUSET;
    memcpy(pdu->Parameter,strName.toStdString().c_str(),32);
    memcpy(pdu->Parameter+32,strPWD.toStdString().c_str(),32);

    sendPDU(pdu);
}

void Client::FlushFriendList()
{
    //创建协议数据单元并发送至服务器
    PDU*pdu=mkPDU(0);
    pdu->uiMSGType=ENUM_MSG_TYPE_FLUSHFRIENDLIST_REQUEST;
    memcpy(pdu->Parameter,strUserName.toStdString().c_str(),32);

    sendPDU(pdu);
}

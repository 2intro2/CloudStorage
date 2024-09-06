
#include "mytcpsocket.h"
#include "operatordb.h"
#include "protocol.h"

MyTcpSocket::MyTcpSocket()
{
    handler=new MsgHandler();
    //关联信号和槽(服务器一旦收到消息就开始接收)
    connect(this,&QTcpSocket::readyRead,this,&MyTcpSocket::recvMSG);
    //关联信号和槽(客户端一旦断开连接就开始处理)
    connect(this,&QTcpSocket::disconnected,this,&MyTcpSocket::dicClient);
}

MyTcpSocket::~MyTcpSocket()
{
    delete handler;
}

void MyTcpSocket::sendPDU(PDU *respdu)
{
    this->write((char*)respdu,respdu->uiPDULen);
    //释放空间
    free(respdu);
    respdu=NULL;
}

PDU *MyTcpSocket::handleMsg(PDU *pdu)
{
    //打印协议数据单元存储的信息(debug)
    qDebug()<<"协议数据单元大小(字节):"<<pdu->uiPDULen<<endl
            <<"实际消息长度:"<<pdu->uiMSgLen<<endl
            <<"消息类型:"<<pdu->uiMSGType<<endl
            <<"用户名:"<<pdu->Parameter<<endl
            <<"密码:"<<pdu->Parameter+32<<endl
            <<"消息:"<<pdu->MSG<<endl;

    //依据消息类型 选择处理消息的方法
    switch (pdu->uiMSGType)
    {
        //处理客户端的注册请求
        case ENUM_MSG_TYPE_REGIST_REQUSET:
           return handler->FC_REGIST(pdu);
        //处理客户端的登录请求
        case ENUM_MSG_TYPE_LOGIN_REQUSET:
           return handler->FC_LOGIN(pdu,&strUserName);
        //好友界面-处理添加好友请求
        case ENUM_MSG_TYPE_FINDUSER_REQUEST:
           return handler->FC_FINDUSER(pdu);
        //好友界面-处理查看在线用户请求
        case ENUM_MSG_TYPE_ONLINEUSERS_REQUEST:
           return handler->FC_ONLINEUSERS();
        //好友界面-处理发送好友邀请请求(在线用户界面)
        case ENUM_MSG_TYPE_SENDINVATION_REQUEST:
           return handler->FC_SENDINVATION(pdu);
        //好友界面-处理接受好友邀请
        case ENUM_MSG_TYPE_ACCEPTINVATION_REQUEST:
           return handler->FC_ACCEPTINVATION(pdu);
        //刷新好友列表
        case ENUM_MSG_TYPE_FLUSHFRIENDLIST_REQUEST:
           return handler->FC_FLUSHFRIENDLIST(pdu);
        //刷新好友列表
        case ENUM_MSG_TYPE_DELETEFRIEND_REQUEST:
           return handler->FC_DELETEFRIEND(pdu);
        //处理客户端向目标用户发送消息的请求
        case ENUM_MSG_TYPE_CHAT_REQUEST:
           handler->FC_CHAT(pdu);
           break;
        //文件界面-创建文件夹
        case ENUM_MSG_TYPE_MKDIR_REQUEST:
           return handler->FC_MKDIR(pdu);
        //文件界面-刷新文件
        case ENUM_MSG_TYPE_FLUSHFILE_REQUEST:
           return handler->FC_FLUSHFILE(pdu);
        //文件界面-删除文件夹
        case ENUM_MSG_TYPE_DELDIR_REQUEST:
           return handler->FC_DELDIR(pdu);
        //文件界面-删除文件
        case ENUM_MSG_TYPE_DELFILE_REQUEST:
           return handler->FC_DELFILE(pdu);
        //文件界面-重命名文件
        case ENUM_MSG_TYPE_RENAME_REQUEST:
           return handler->FC_RENAME(pdu);
        //文件界面-移动文件
        case ENUM_MSG_TYPE_MOVEFILE_REQUEST:
           return handler->FC_MOVEFILE(pdu);
        //文件界面-上传文件
        case ENUM_MSG_TYPE_UPLOADFILE_REQUEST:
           return handler->FC_UPLOADFILE(pdu);
        //文件界面-上传文件数据
        case ENUM_MSG_TYPE_UPLOADFILEDATA_REQUEST:
           return handler->FC_UPLOADFILEDATA(pdu);
        //文件界面-下载文件
        case ENUM_MSG_TYPE_DOWNLOADFILE_REQUEST:
           return handler->FC_DOWNLOADFILE(pdu);
        //文件界面-下载文件数据
        case ENUM_MSG_TYPE_DOWNLOADFILEDATA_REQUEST:
           return handler->FC_DOWNLOADFILEDATA(this);
        //文件界面-分享文件
        case ENUM_MSG_TYPE_SHAREFILE_REQUEST:
           return handler->FC_SHAREFILE(pdu);
        //文件界面-分享文件(接收方)
        case ENUM_MSG_TYPE_SHAREFILEAGREE_REQUEST:
           return handler->FC_SHAREFILEAGREE(pdu);
        default: break;
    }

    return nullptr;
}

void MyTcpSocket::recvMSG()
{
    //设置接收数据的缓冲区 处理粘包和半包的问题
    //接收此次客户端发送的全部数据
    QByteArray CurData=this->readAll();
    //将上一次未处理的数据和这一次接收的数据合并
    Buffer.append(CurData);

    //循环处理粘包数据  注意每一次处理Buffer BUffer的开头必为pdu的头部
    while(Buffer.size()>=(int)sizeof(PDU)) //通过是否含有PDU的头部来初步判断Buffer的数据是否仍为半包
    {
        //一次读取一个协议数据单元
        PDU* pdu=(PDU*) Buffer.data();
        //通过判断PDU中的数据是否完整来判断Buffer的数据是否为半包
        if(Buffer.size()<(int)pdu->uiPDULen) break;

        PDU* respdu=handleMsg(pdu);
        if(respdu) sendPDU(respdu);

        //释放缓冲区(清除以处理的数据)
        Buffer.remove(0,pdu->uiPDULen);
    }
}

void MyTcpSocket::dicClient()
{
    //数据库处理用户下线操作
    OperatorDB::getInstance().handleOFFLine(strUserName.toStdString().c_str());
    //发出信号 通知服务器该客户端已断开连接 移除该客户端使用的socket
    emit dicedClient(this);
}

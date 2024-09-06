#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include "protocol.h"

#include <QString>

class MsgHandler
{
public:
    MsgHandler();
    //注册功能的实现
    void FC_REGIST(PDU* pdu);
    //登录功能的实现
    void FC_LOGIN(PDU* pdu);
    //查找用户功能的实现
    void FC_FINDUSER(PDU* pdu);
    //发送好友邀请功能的实现(发送)
    void FC_SENDINVATION(PDU* pdu);
    //发送好友邀请功能的实现(接收)
    void FC_HANDLEINVATION(PDU* pdu);
    //接受好友的实现(接收)
    void FC_ACCEPTINVATION(PDU* pdu);
    //在线用户功能
    void FC_ONLINEUSERS(PDU* pdu,QString* strUserName);
    //刷新好友功能的实现
    void FC_FLUSHFRIENDLIST(PDU* pdu);
    //删除好友功能的实现
    void FC_DELETEFRIEND(PDU* pdu);
    //好友聊天功能的实现
    void FC_CHAT(PDU* pdu);
    //创建文件夹功能的实现
    void FC_MKDIR(PDU* pdu);
    //刷新文件功能的实现
    void FC_FLUSHFILE(PDU* pdu);
    //删除文件夹功能的实现
    void FC_DELDIR(PDU* pdu);
    //删除文件功能的实现
    void FC_DELFILE(PDU* pdu);
    //重命名文件功能的实现
    void FC_RENAME(PDU* pdu);
    //移动文件功能的实现
    void FC_MOVEFILE(PDU* pdu);
    //上传文件功能的实现
    void FC_UPLOADFILE(PDU* pdu);
    //上传文件数据功能的实现
    void FC_UPLOADFILEDATA(PDU* pdu);
    //下载文件功能的实现
    void FC_DOWNLOADFILE(PDU* pdu);
    //下载文件数据功能的实现
    void FC_DOWNLOADFILEDATA(PDU* pdu);
    //分享文件功能的实现
    void FC_SHAREFILE(PDU* pdu);
    //分享文件功能的实现(接收方)
    void FC_SHAREFILEREC(PDU* pdu);
    //分享文件功能的实现(接收方-反馈)
    void FC_SHAREFILERECRES(PDU* pdu);
};

#endif // MSGHANDLER_H

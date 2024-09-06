#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include "protocol.h"

#include <QFile>
#include <QString>

class MyTcpSocket; //前置声明 避免头文件循环引用

class MsgHandler
{
    //上传文件时需要的临时变量
    QFile UploadFile;
    bool isUpload;
    qint64 UploadFileSize;
    qint64 UploadFileRec;

    //下载文件时需要的临时变量
    QFile DownloadFile;
    bool isDownload;

public:
    MsgHandler();
    //注册功能的实现
    PDU* FC_REGIST(PDU* pdu);
    //登录功能的实现
    PDU* FC_LOGIN(PDU* pdu,QString* strUserName);
    //查找用户功能
    PDU* FC_FINDUSER(PDU* pdu);
    //在线用户功能
    PDU* FC_ONLINEUSERS();
    //发送好友邀请功能(发送)
    PDU* FC_SENDINVATION(PDU*pdu);
    //发送好友邀请功能(发送)
    PDU* FC_ACCEPTINVATION(PDU*pdu);
    //刷新好友列表功能的实现
    PDU* FC_FLUSHFRIENDLIST(PDU*pdu);
    //删除好友功能的实现
    PDU* FC_DELETEFRIEND(PDU*pdu);
    //好友聊天功能的实现
    void FC_CHAT(PDU*pdu);
    //创建文件夹功能的实现
    PDU* FC_MKDIR(PDU*pdu);
    //刷新文件功能的实现
    PDU* FC_FLUSHFILE(PDU*pdu);
    //删除文件夹功能的实现
    PDU* FC_DELDIR(PDU*pdu);
    //删除文件功能的实现
    PDU* FC_DELFILE(PDU* pdu);
    //重命名文件功能的实现
    PDU* FC_RENAME(PDU* pdu);
    //移动文件功能的实现
    PDU* FC_MOVEFILE(PDU* pdu);
    //上传文件功能的实现
    PDU* FC_UPLOADFILE(PDU* pdu);
    //上传文件数据功能的实现
    PDU* FC_UPLOADFILEDATA(PDU* pdu);
    //下载文件功能的实现
    PDU* FC_DOWNLOADFILE(PDU* pdu);
    //下载文件数据功能的实现
    PDU* FC_DOWNLOADFILEDATA(MyTcpSocket* socket);
    //分享文件功能的实现
    PDU* FC_SHAREFILE(PDU* pdu);
    //分享文件功能的实现(接收方)
    PDU* FC_SHAREFILEAGREE(PDU* pdu);

};

#endif // MSGHANDLER_H

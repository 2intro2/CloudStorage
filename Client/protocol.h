#ifndef PROTOCOL_H
#define PROTOCOL_H
/*****************************************通信协议*****************************************/
/*
 * PDU(协议数据单元):在网络协议中通过定义PDU规定传输和接收设备之间发送的数据的格式和结构。
 * 柔性数组:结构体最后一个成员可以是一个长度不确定的数组 该数组就被称为柔性数组 柔性数组不占结构体的大小
*/
typedef unsigned int uint;

enum ENUM_MSG_TYPE //消息类型的枚举值
{
    ENUM_MSG_TYPE_MIN=0,
    //注册
    ENUM_MSG_TYPE_REGIST_REQUSET,
    ENUM_MSG_TYPE_REGIST_RESPOND,
    //登录
    ENUM_MSG_TYPE_LOGIN_REQUSET,
    ENUM_MSG_TYPE_LOGIN_RESPOND,
    //好友界面相关功能
    //查找用户
    ENUM_MSG_TYPE_FINDUSER_REQUEST,
    ENUM_MSG_TYPE_FINDUSER_RESPOND,
    //发送好友请求
    ENUM_MSG_TYPE_SENDINVATION_REQUEST,
    ENUM_MSG_TYPE_SENDINVATION_RESPOND,
    //接受好友请求
    ENUM_MSG_TYPE_ACCEPTINVATION_REQUEST,
    ENUM_MSG_TYPE_ACCEPTINVATION_RESPOND,
    //在线用户
    ENUM_MSG_TYPE_ONLINEUSERS_REQUEST,
    ENUM_MSG_TYPE_ONLINEUSERS_RESPOND,
    //刷新好友列表
    ENUM_MSG_TYPE_FLUSHFRIENDLIST_REQUEST,
    ENUM_MSG_TYPE_FLUSHFRIENDLIST_RESPOND,
    //删除好友
    ENUM_MSG_TYPE_DELETEFRIEND_REQUEST,
    ENUM_MSG_TYPE_DELETEFRIEND_RESPOND,
    //发送消息
    ENUM_MSG_TYPE_CHAT_REQUEST,
    ENUM_MSG_TYPE_CHAT_RESPOND,
    //文件界面相关功能
    //创建文件夹
    ENUM_MSG_TYPE_MKDIR_REQUEST,
    ENUM_MSG_TYPE_MKDIR_RESPOND,
    //刷新文件
    ENUM_MSG_TYPE_FLUSHFILE_REQUEST,
    ENUM_MSG_TYPE_FLUSHFILE_RESPOND,
    //删除文件夹
    ENUM_MSG_TYPE_DELDIR_REQUEST,
    ENUM_MSG_TYPE_DELDIR_RESPOND,
    //删除文件
    ENUM_MSG_TYPE_DELFILE_REQUEST,
    ENUM_MSG_TYPE_DELFILE_RESPOND,
    //重命名文件
    ENUM_MSG_TYPE_RENAME_REQUEST,
    ENUM_MSG_TYPE_RENAME_RESPOND,
    //移动文件
    ENUM_MSG_TYPE_MOVEFILE_REQUEST,
    ENUM_MSG_TYPE_MOVEFILE_RESPOND,
    //上传文件
    ENUM_MSG_TYPE_UPLOADFILE_REQUEST,
    ENUM_MSG_TYPE_UPLOADFILE_RESPOND,
    //上传文件数据
    ENUM_MSG_TYPE_UPLOADFILEDATA_REQUEST,
    ENUM_MSG_TYPE_UPLOADFILEDATA_RESPOND,
    //下载文件
    ENUM_MSG_TYPE_DOWNLOADFILE_REQUEST,
    ENUM_MSG_TYPE_DOWNLOADFILE_RESPOND,
    //下载文件数据
    ENUM_MSG_TYPE_DOWNLOADFILEDATA_REQUEST,
    ENUM_MSG_TYPE_DOWNLOADFILEDATA_RESPOND,
    //分享文件
    ENUM_MSG_TYPE_SHAREFILE_REQUEST,
    ENUM_MSG_TYPE_SHAREFILE_RESPOND,
    //分享文件(接收方)
    ENUM_MSG_TYPE_SHAREFILEAGREE_REQUEST,
    ENUM_MSG_TYPE_SHAREFILEAGREE_RESPOND,
    ENUM_MSG_TYPE_MAX=0x3f3f3f3f

};

struct PDU //定义协议数据单元
{
    uint uiPDULen; //协议数据单元的总长度
    uint uiMSgLen; //实际消息的总长度
    uint uiMSGType; //消息类型
    char Parameter[64]; //参数
    char MSG[]; //实际消息

};

struct FileInfo //定义文件信息结构体
{
    char FileName[32]; //文件名
    uint FileType; //文件类型
};

PDU* mkPDU(uint uiMSGLen); //创建协议数据单元

#endif // PROTOCOL_H

#include "msghandler.h"
#include "operatordb.h"
#include "server.h"

#include <QDir>
#include <QtDebug>

MsgHandler::MsgHandler()
{
    isUpload=false;
    isDownload=false;
}

PDU *MsgHandler::FC_REGIST(PDU* pdu)
{
    char paName[32]="";
    char paPWD[32]="";
    memcpy(paName,pdu->Parameter,32);
    memcpy(paPWD,pdu->Parameter+32,32);

    qDebug()<<"注册"<<endl
            <<"paName:"<<paName<<"paPWD"<<paPWD<<endl; //debug

    //数据库处理注册请求
    bool res=OperatorDB::getInstance().handleRegist(paName,paPWD);

    //注册成功则在文件系统中创建属于该用户的文件目录
    if(res==true)
    {
        QDir dir; //利用目录对象创建目录
        dir.mkdir(QString("%1/%2")
                  .arg(server::getInstance().getRootPath())
                  .arg(paName));
    }
    //向客户端反馈请求结果
    PDU* respdu=mkPDU(0);
    respdu->uiMSGType=ENUM_MSG_TYPE_REGIST_RESPOND; //标注消息类型
    memcpy(respdu->Parameter,&res,sizeof(res));

    return respdu;
}

PDU *MsgHandler::FC_LOGIN(PDU *pdu, QString* strUserName)
{
    char paName[32]="";
    char paPWD[32]="";
    memcpy(paName,pdu->Parameter,32);
    memcpy(paPWD,pdu->Parameter+32,32);

    qDebug()<<"登录"<<endl
            <<"paName:"<<paName<<"paPWD"<<paPWD<<endl; //debug

    //数据库处理登录请求
    bool res=OperatorDB::getInstance().handleLogin(paName,paPWD);
    if(res) *strUserName=paName;
    //向客户端反馈请求结果
    PDU* respdu=mkPDU(0);
    respdu->uiMSGType=ENUM_MSG_TYPE_LOGIN_RESPOND; //标注消息类型
    memcpy(respdu->Parameter,&res,sizeof(res));

    return respdu;
}

PDU *MsgHandler::FC_FINDUSER(PDU *pdu)
{
    char paName[32]="";
    memcpy(paName,pdu->Parameter,32);

    //数据库查找用户
    int res=OperatorDB::getInstance().handleFindUser(paName);

    //向客户端反馈请求结果
    PDU* respdu=mkPDU(0);
    respdu->uiMSGType=ENUM_MSG_TYPE_FINDUSER_RESPOND; //标注消息类型
    memcpy(respdu->Parameter,&res,sizeof(res));
    memcpy(respdu->Parameter+4,paName,32);

    return respdu;
}

PDU *MsgHandler::FC_ONLINEUSERS()
{
    //数据库查找在线用户
    QStringList onlineuserslist;
    OperatorDB::getInstance().handleFindOnlineUsers(onlineuserslist);
    //向客户端反馈请求结果
    PDU* respdu=mkPDU(onlineuserslist.size()*32);
    respdu->uiMSGType=ENUM_MSG_TYPE_ONLINEUSERS_RESPOND; //标注消息类型
    for(int i=0;i<onlineuserslist.size();i++)
    {
        const char*temp=onlineuserslist[i].toStdString().c_str();
        memcpy(respdu->MSG+i*32,temp,32);
    }

    return respdu;
}

PDU *MsgHandler::FC_SENDINVATION(PDU *pdu)
{
    char strCurName[32]="";
    char strTarName[32]="";
    memcpy(strCurName,pdu->Parameter,32);
    memcpy(strTarName,pdu->Parameter+32,32);

    //数据库判断这份好友邀请是否合法
    int res=OperatorDB::getInstance().handleFriendInvation(strCurName,strTarName);
    qDebug()<<"FC_SENDINVATION res:"<<res;
    //好友邀请合法 发送好友邀请
    if(res==1)
    {
       MyTcpServer::getInstance().resend(strTarName,pdu);
    }
    //向客户端反馈请求结果
    PDU* respdu=mkPDU(0);
    respdu->uiMSGType=ENUM_MSG_TYPE_SENDINVATION_RESPOND; //标注消息类型
    memcpy(respdu->Parameter,&res,sizeof(res));

    return respdu;
}

PDU *MsgHandler::FC_ACCEPTINVATION(PDU *pdu)
{
    char strCurName[32]="";
    char strTarName[32]="";
    memcpy(strCurName,pdu->Parameter,32);
    memcpy(strTarName,pdu->Parameter+32,32);

    //数据库添加好友关系
    OperatorDB::getInstance().handleAddFriendship(strCurName,strTarName);
    //向客户端(被添加方)反馈是否添加成功
    PDU* respdu=mkPDU(0);
    memcpy(respdu->Parameter,pdu->Parameter,64);
    respdu->uiMSGType=ENUM_MSG_TYPE_ACCEPTINVATION_RESPOND; //标注消息类型
    //向客户端(添加方)转发是否添加成功
    MyTcpServer::getInstance().resend(strCurName,respdu);

    return respdu;
}

PDU *MsgHandler::FC_FLUSHFRIENDLIST(PDU *pdu)
{
    char strCurName[32]=""; memcpy(strCurName,pdu->Parameter,32);
    //数据库获取该用户的所有在线好友
    QStringList FriendList;
    OperatorDB::getInstance().handleFindFriends(strCurName,FriendList);
    //向客户端返回该用户的所有在线好友
    PDU* respdu=mkPDU(FriendList.size()*32);
    respdu->uiMSGType=ENUM_MSG_TYPE_FLUSHFRIENDLIST_RESPOND; //标注消息类型
    for(int i=0;i<FriendList.size();i++)
    {
        const char*temp=FriendList[i].toStdString().c_str();
        memcpy(respdu->MSG+i*32,temp,32);
    }

    return respdu;
}

PDU *MsgHandler::FC_DELETEFRIEND(PDU *pdu)
{
    char strCurName[32]="";
    char strTarName[32]="";
    memcpy(strCurName,pdu->Parameter,32);
    memcpy(strTarName,pdu->Parameter+32,32);

    //数据库删除好友关系
    int res=OperatorDB::getInstance().handleDeleteFriendship(strCurName,strTarName);
    //向客户端反馈删除结果
    PDU* respdu=mkPDU(0);
    memcpy(respdu->Parameter,&res,sizeof(res));
    respdu->uiMSGType=ENUM_MSG_TYPE_DELETEFRIEND_RESPOND; //标注消息类型

    return respdu;
}

void MsgHandler::FC_CHAT(PDU *pdu)
{
    char TarFriend[32]=""; memcpy(TarFriend,pdu->Parameter+32,32);
    pdu->uiMSGType=ENUM_MSG_TYPE_CHAT_RESPOND;

    //将消息转发给目标好友
    MyTcpServer::getInstance().resend(TarFriend,pdu);
}

PDU *MsgHandler::FC_MKDIR(PDU *pdu)
{
    //获取当前路径和文件夹名 并进行拼接
    QString CurPath=pdu->MSG;
    char NewDirName[32]=""; memcpy(NewDirName,pdu->Parameter,32);
    QString NewPath=QString("%1/%2").arg(CurPath).arg(NewDirName);


    // res: -1(创建文件夹失败) 0(存在同名文件夹) 1(创建文件夹成功)
    //判断当前路径和文件名是否合法 并创建文件夹
    int res=1;
    QDir dir;
    if(!dir.exists(CurPath)) res=-1;
    else if(dir.exists(NewPath)) res=0;
    else if(!dir.mkdir(NewPath)) res=-1;

    //向客户端反馈创建结果
    PDU* respdu=mkPDU(0);
    memcpy(respdu->Parameter,&res,sizeof(res));
    respdu->uiMSGType=ENUM_MSG_TYPE_MKDIR_RESPOND; //标注消息类型

    return respdu;
}

PDU *MsgHandler::FC_FLUSHFILE(PDU *pdu)
{
    //获取当前路径
    char* pCurPath=pdu->MSG;
    //创建Qdir对象获取当前路径下的文件信息
    QDir dir(pCurPath);
    QFileInfoList FileInfoList=dir.entryInfoList();
    int FileCnt=FileInfoList.size();

    //将该目录下的所有文件信息(文件名 文件类型)发送给客户端
    PDU* respdu=mkPDU(FileCnt*sizeof(FileInfo));
    respdu->uiMSGType=ENUM_MSG_TYPE_FLUSHFILE_RESPOND;

    //依次填充文件信息到协议数据单元中
    FileInfo* pFileInfo=NULL;
    QString FileName;
    for(int i=0;i<FileCnt;i++)
    {
        pFileInfo=(FileInfo*) respdu->MSG+i;
        FileName=FileInfoList[i].fileName();
        memcpy(pFileInfo->FileName,FileName.toStdString().c_str(),32);
        //文件类型  0:目录  1:文件
        if(FileInfoList[i].isDir()) pFileInfo->FileType=0;
        else if(FileInfoList[i].isFile()) pFileInfo->FileType=1;

        qDebug()<<"filename"<<FileName<<endl;
    }

    return respdu;
}

PDU *MsgHandler::FC_DELDIR(PDU *pdu)
{
    //获取当前路径和文件夹名 并进行拼接
    QString CurPath=pdu->MSG;
    char DirName[32]=""; memcpy(DirName,pdu->Parameter,32);
    QString DelPath=QString("%1/%2").arg(CurPath).arg(DirName);

    //创建Qdir对象删除该文件夹
    QDir dir(DelPath);
    bool res=dir.removeRecursively();

    //向客户端反馈创建结果
    PDU* respdu=mkPDU(0);
    memcpy(respdu->Parameter,&res,sizeof(res));
    respdu->uiMSGType=ENUM_MSG_TYPE_DELDIR_RESPOND; //标注消息类型

    return respdu;
}

PDU *MsgHandler::FC_DELFILE(PDU *pdu)
{
    //获取当前路径和文件名 并进行拼接
    QString CurPath=pdu->MSG;
    char FileName[32]=""; memcpy(FileName,pdu->Parameter,32);
    QString DelPath=QString("%1/%2").arg(CurPath).arg(FileName);

    //创建Qdir对象删除该文件
    QDir dir;
    bool res=dir.remove(DelPath);

    //向客户端反馈创建结果
    PDU* respdu=mkPDU(0);
    memcpy(respdu->Parameter,&res,sizeof(res));
    respdu->uiMSGType=ENUM_MSG_TYPE_DELFILE_RESPOND; //标注消息类型

    return respdu;
}

PDU *MsgHandler::FC_RENAME(PDU *pdu)
{
    //获取当前路径和新旧文件名 并分别进行拼接
    QString CurPath=pdu->MSG;
    char OldName[32]=""; memcpy(OldName,pdu->Parameter,32);
    char NewName[32]=""; memcpy(NewName,pdu->Parameter+32,32);

    QString OldPath=QString("%1/%2").arg(CurPath).arg(OldName);
    QString NewPath=QString("%1/%2").arg(CurPath).arg(NewName);


    //创建Qdir对象重命名该文件
    QDir dir;
    bool res=dir.rename(OldPath,NewPath);

    //向客户端反馈创建结果
    PDU* respdu=mkPDU(0);
    memcpy(respdu->Parameter,&res,sizeof(res));
    respdu->uiMSGType=ENUM_MSG_TYPE_RENAME_RESPOND; //标注消息类型

    return respdu;
}

PDU *MsgHandler::FC_MOVEFILE(PDU *pdu)
{
    //获取移动文件的源路径和目标路径
    int sizeSrcPath=0,sizeTarPath=0;
    memcpy(&sizeSrcPath,pdu->Parameter,sizeof(int));
    memcpy(&sizeTarPath,pdu->Parameter+sizeof(int),sizeof(int));

    char* SrcPath=new char[sizeSrcPath+1]; memset(SrcPath,0,sizeSrcPath+1); memcpy(SrcPath,pdu->MSG,sizeSrcPath);
    char* TarPath=new char[sizeTarPath+1]; memset(TarPath,0,sizeTarPath+1); memcpy(TarPath,pdu->MSG+sizeSrcPath,sizeTarPath);

    //通过Dir对象移动文件
    QDir dir;
    bool res=dir.rename(SrcPath,TarPath);

    //向客户端反馈创建结果
    PDU* respdu=mkPDU(0);
    memcpy(respdu->Parameter,&res,sizeof(res));
    respdu->uiMSGType=ENUM_MSG_TYPE_MOVEFILE_RESPOND; //标注消息类型

    //释放内存
    free(SrcPath);
    free(TarPath);

    return respdu;
}

PDU *MsgHandler::FC_UPLOADFILE(PDU *pdu)
{
    qDebug()<<"Upload Start";
    //res用于返回服务器处理的结果  -1(已有文件正在上传) 0(在网盘中创建文件失败) 1(可以进行上传)
    int res=0;
    PDU* respdu=mkPDU(0);
    respdu->uiMSGType=ENUM_MSG_TYPE_UPLOADFILE_RESPOND; //标注消息类型

    if(isUpload)
    {
        qDebug()<<"Upload is existing";
        res=-1; memcpy(respdu->Parameter,&res,sizeof(res));
        return respdu;
    }

    //获取上传文件信息
    char UploadFileName[32]="";
    qint64 FileSize=0;
    memcpy(UploadFileName,pdu->Parameter,32);
    memcpy(&FileSize,pdu->Parameter+32,sizeof(qint64));

    //创建文件对象操作文件
    qDebug()<<"FilePath"<<QString("%1/%2").arg(pdu->MSG).arg(UploadFileName);
    UploadFile.setFileName(QString("%1/%2").arg(pdu->MSG).arg(UploadFileName));

    if(UploadFile.open(QIODevice::WriteOnly))
    {
        //记录相关信息
        UploadFileSize=FileSize;
        UploadFileRec=0;
        isUpload=true;

        qDebug()<<"UploadFileSize"<<UploadFileSize<<endl
        <<"UploadFileRec"<<UploadFileRec<<endl
        <<"isUpload"<<isUpload;
    }
    else
    {
        qDebug()<<"Create  is failed";
        res=0; memcpy(respdu->Parameter,&res,sizeof(res));
        return respdu;
    }

    res=1; memcpy(respdu->Parameter,&res,sizeof(res));
    return respdu;

}

PDU *MsgHandler::FC_UPLOADFILEDATA(PDU *pdu)
{
    //将数据写入网盘文件中
    UploadFile.write(pdu->MSG,pdu->uiMSgLen);
    UploadFileRec+=pdu->uiMSgLen;

    //数据没有完全上传就继续
    if(UploadFileRec<UploadFileSize) return NULL;

    //完全上传后反馈给客户端
    isUpload=false;
    UploadFile.close();

    PDU* respdu=mkPDU(0);
    bool res=UploadFileRec==UploadFileSize; memcpy(respdu->Parameter,&res,sizeof(res));
    respdu->uiMSGType=ENUM_MSG_TYPE_UPLOADFILEDATA_RESPOND; //标注消息类型

    return respdu;
}

PDU *MsgHandler::FC_DOWNLOADFILE(PDU *pdu)
{
    qDebug()<<"Download Start";

    //res用于返回服务器处理的结果  -1(已有文件正在上传) 0(打开文件失败) 1(可以进行下载)
    int res=0;
    PDU* respdu=mkPDU(0);
    respdu->uiMSGType=ENUM_MSG_TYPE_DOWNLOADFILE_RESPOND; //标注消息类型

    //判断是否有文件正在下载
    if(isDownload)
    {
        qDebug()<<"Download is existing";
        res=-1; memcpy(respdu->Parameter,&res,sizeof(res));
        return respdu;
    }

    QFileInfo FileInfo(pdu->MSG);
    qint64 FileSize=FileInfo.size();

    //创建文件对象操作文件
    qDebug()<<"FilePath"<<pdu->MSG<<endl
            <<"FileSize"<<FileSize;
    DownloadFile.setFileName(pdu->MSG);

    //判断文件是否打开成功
    if(DownloadFile.open(QIODevice::ReadOnly)==false)
    {
        qDebug()<<"Open is failed";
        res=0; memcpy(respdu->Parameter,&res,sizeof(res));
        return respdu;
    }
    else
    {
        res=1; memcpy(respdu->Parameter,&res,sizeof(res));
        memcpy(respdu->Parameter+sizeof(res),&FileSize,sizeof(FileSize));
    }

    return respdu;
}

PDU *MsgHandler::FC_DOWNLOADFILEDATA(MyTcpSocket *socket)
{
    //更新下载状态
    isDownload=true;

    //规定每次上传数据的大小(提高安全性)
    PDU* filedata=mkPDU(4096);
    filedata->uiMSGType=ENUM_MSG_TYPE_DOWNLOADFILEDATA_RESPOND;

    //循环上传文件数据
    while(true)
    {
        int res=DownloadFile.read(filedata->MSG,4096);
        if(res<=0) break;

        //每一次并不一定能够读取4096字节的数据 所以要计算一下正确的消息长度和pdu长度
        filedata->uiMSgLen=res;
        filedata->uiPDULen=sizeof(PDU)+res;
        //将数据发送给服务器
        socket->write((char*)filedata,filedata->uiPDULen);
    }

    //更新下载状态
    isDownload=false;
    //关闭文件 释放空间
    DownloadFile.close();
    free(filedata);
    filedata=NULL;

    return NULL;

}

PDU *MsgHandler::FC_SHAREFILE(PDU *pdu)
{
    //获取分享文件用户信息
    char CurName[32]=""; memcpy(CurName,pdu->Parameter,32);
    int friendcnt=0; memcpy(&friendcnt,pdu->Parameter+32,sizeof(int));

    //创建协议数据单元 并按分享好友的顺序将该单元依次进行转发
    PDU* rsendPDU=mkPDU(pdu->uiMSgLen-friendcnt*32);
    rsendPDU->uiMSGType=pdu->uiMSGType;

    memcpy(rsendPDU->Parameter,CurName,32); //分享文件用户名
    memcpy(rsendPDU->MSG,pdu->MSG+friendcnt*32,pdu->uiMSgLen-friendcnt*32); //分享文件所在路径

    //转发
    for(int i=0;i<friendcnt;i++)
    {
        char TarName[32]=""; memcpy(TarName,pdu->MSG+i*32,32);
        MyTcpServer::getInstance().resend(TarName,rsendPDU);
    }

    //转发完毕后反馈给客户端
    PDU* respdu=mkPDU(0);
    respdu->uiMSGType=ENUM_MSG_TYPE_SHAREFILE_RESPOND; //标注消息类型

    return respdu;
}

PDU *MsgHandler::FC_SHAREFILEAGREE(PDU *pdu)
{
    //获取分享文件路径和保存文件路径(默认为用户根目录)
    QString ShareFilePath=QString(pdu->MSG);
    QString FileName=ShareFilePath.right(ShareFilePath.size()-ShareFilePath.lastIndexOf('/')-1);
    QString SaveFilePath=QString("%1/%2/%3").arg(server::getInstance().getRootPath()).arg(pdu->Parameter).arg(FileName);
    qDebug()<<"ShareFilePath"<<ShareFilePath<<endl
            <<"SaveFilePath"<<SaveFilePath;

    //在文件系统中复制文件
    QFileInfo fileinfo(ShareFilePath);
    bool res=true;

    if(fileinfo.isFile())
    {
        res=QFile::copy(ShareFilePath,SaveFilePath);
    }
    else if(fileinfo.isDir())
    {
        //复制目录
    }

    //反馈客户端
    PDU* respdu=mkPDU(0);
    respdu->uiMSGType=ENUM_MSG_TYPE_SHAREFILEAGREE_RESPOND; //标注消息类型
    memcpy(respdu->Parameter,&res,sizeof(res));

    return respdu;
}








































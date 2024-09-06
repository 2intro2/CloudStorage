#include "client.h"
#include "index.h"
#include "msghandler.h"

#include <QMessageBox>

MsgHandler::MsgHandler()
{

}

void MsgHandler::FC_REGIST(PDU *pdu)
{
    //读取注册请求的结果,并反馈给用户
    bool res; memcpy(&res,pdu->Parameter,sizeof(res));
    if(res)  QMessageBox::information(&Client::getInstance(),"Qt网盘","注册成功!");
    else QMessageBox::information(&Client::getInstance(),"Qt网盘","注册失败");
}

void MsgHandler::FC_LOGIN(PDU *pdu)
{
    //读取登录请求的结果,并反馈给用户
    bool res; memcpy(&res,pdu->Parameter,sizeof(res));
    if(res)
    {
        //登录成功后 跳转至正式界面 并隐藏登录界面
        Index::getInstance().show();
        Client::getInstance().hide();
    }
    else QMessageBox::information(&Client::getInstance(),"Qt网盘","登录失败");
}

void MsgHandler::FC_FINDUSER(PDU *pdu)
{

    //读取服务器返回的结果,并反馈给用户
    int res; memcpy(&res,pdu->Parameter,sizeof(res));
    char TarName[32]=""; memcpy(TarName,pdu->Parameter+4,32);
    if(res==-1) QMessageBox::information(&Index::getInstance(),"Qt网盘",QString("用户%1不存在").arg(TarName));
    else if(res==0) QMessageBox::information(&Index::getInstance(),"Qt网盘",QString("用户%1离线中").arg(TarName));
    else if(res==1)
    {
        int ret=QMessageBox::information(&Index::getInstance(),"Qt网盘",QString("用户%1在线").arg(TarName),"添加好友","取消");
        qDebug()<<ret;
        if(ret!=0) return;
        //发送好友邀请
        PDU* respdu=mkPDU(0);
        respdu->uiMSGType=ENUM_MSG_TYPE_SENDINVATION_REQUEST;
        memcpy(respdu->Parameter,Client::getInstance().getUserName().toStdString().c_str(),32);
        memcpy(respdu->Parameter+32,TarName,32);

        Client::getInstance().sendPDU(respdu);
    }
}

void MsgHandler::FC_SENDINVATION(PDU *pdu)
{
    //读取服务器返回的结果,并反馈给用户
    int res; memcpy(&res,pdu->Parameter,sizeof(res));
    if(res==-2) QMessageBox::information(&Index::getInstance(),"Qt网盘","ERROR");
    else if(res==-1) QMessageBox::information(&Index::getInstance(),"Qt网盘","你们已经是好友了");
    else if(res==0)  QMessageBox::information(&Index::getInstance(),"Qt网盘","对方离线中,发送邀请失败");
    else if(res==1)  QMessageBox::information(&Index::getInstance(),"Qt网盘","发送邀请成功");
}

void MsgHandler::FC_HANDLEINVATION(PDU *pdu)
{
    char CurName[32]="";
    memcpy(CurName,pdu->Parameter,32);
    int res=QMessageBox::question(&Index::getInstance(),"好友请求",QString("用户%1请求添加好友").arg(CurName));
    if(res!=QMessageBox::Yes) return;
    //接受好友邀请
    PDU* respdu=mkPDU(0);
    memcpy(respdu->Parameter,pdu->Parameter,64);
    respdu->uiMSGType=ENUM_MSG_TYPE_ACCEPTINVATION_REQUEST;

    Client::getInstance().sendPDU(respdu);
}

void MsgHandler::FC_ACCEPTINVATION(PDU *pdu)
{
    char Name1[32]="";
    char Name2[32]="";
    memcpy(Name1,pdu->Parameter,32);
    memcpy(Name2,pdu->Parameter+32,32);
    QMessageBox::information(&Index::getInstance(),"Qt网盘",
                             QString("用户%1已成为你的好友")
                             .arg(QString(Name1)==Client::getInstance().getUserName()?Name2:Name1));
    //刷新好友列表
    Client::getInstance().FlushFriendList();
}

void MsgHandler::FC_ONLINEUSERS(PDU *pdu,QString *strUserName)
{
    //读取服务器返回的结果,并反馈给用户
    QStringList onlineuserslist;
    uint cnt=pdu->uiMSgLen/32; //用户名的个数
    for(uint i=0;i<cnt;i++)
    {
        char temp[32]="";
        memcpy(temp,pdu->MSG+i*32,32);
        if(QString(temp)==*strUserName) continue; //不显示用户自己的名字
        onlineuserslist.append(QString(temp));
    }
    Index::getInstance().getPtrFriend()->getPtrOnlineUsers()->updateOnlineUsersList(onlineuserslist);
}

void MsgHandler::FC_FLUSHFRIENDLIST(PDU *pdu)
{
    //读取服务器返回的结果
    QStringList FriendList;
    uint cnt=pdu->uiMSgLen/32; //在线好友个数
    for(uint i=0;i<cnt;i++)
    {
        char temp[32]="";
        memcpy(temp,pdu->MSG+i*32,32);
        FriendList.append(QString(temp));
    }
    //好友界面刷新好友列表
    Index::getInstance().getPtrFriend()->UpdateFriendList(FriendList);
}

void MsgHandler::FC_DELETEFRIEND(PDU *pdu)
{
    int res; memcpy(&res,pdu->Parameter,sizeof(res));
    if(res==-1) QMessageBox::information(&Index::getInstance(),"Qt网盘","删除好友失败");
    else if(res==0) QMessageBox::information(&Index::getInstance(),"Qt网盘","对方已不是你的好友");
    else if(res==1) QMessageBox::information(&Index::getInstance(),"Qt网盘","删除好友成功");
    //刷新好友列表
    Client::getInstance().FlushFriendList();
}

void MsgHandler::FC_CHAT(PDU *pdu)
{
    char TarName[32]=""; memcpy(TarName,pdu->Parameter,32);

    if(Index::getInstance().getPtrFriend()->getPtrChat()->isHidden())Index::getInstance().getPtrFriend()->getPtrChat()->show();

    //更新目标好友
    Index::getInstance().getPtrFriend()->getPtrChat()->TarFriend=QString(TarName);

    //更新聊天内容
    Index::getInstance().getPtrFriend()->getPtrChat()
            ->UpdateChatText(QString("%1: %2").arg(TarName).arg(pdu->MSG));
}

void MsgHandler::FC_MKDIR(PDU *pdu)
{
    int res=0; memcpy(&res,pdu->Parameter,sizeof(res));
    if(res==-1) QMessageBox::information(&Index::getInstance(),"Qt网盘","创建文件夹失败!");
    else if(res==0) QMessageBox::information(&Index::getInstance(),"Qt网盘","该目录下存在同名文件夹!");
    else if(res==1)
    {
        QMessageBox::information(&Index::getInstance(),"Qt网盘","创建文件夹成功");
        Index::getInstance().getPtrFile()->flushfile();
    }
}

void MsgHandler::FC_FLUSHFILE(PDU *pdu)
{
    //获取当前路径下文件的个数
    int FileCnt=pdu->uiMSgLen/sizeof(FileInfo);
    QList<FileInfo*> pFileInfoList;

    //保存每个文件的信息 方便后续操作
    for(int i=0;i<FileCnt;i++)
    {
        FileInfo* pFileInfo=new FileInfo();
        memcpy(pFileInfo,pdu->MSG+i*sizeof(FileInfo),sizeof(FileInfo));
        pFileInfoList.append(pFileInfo);
    }

    //调用文件界面刷新文件列表的函数
    Index::getInstance().getPtrFile()->updatefilelist(pFileInfoList);
}

void MsgHandler::FC_DELDIR(PDU *pdu)
{
    bool res=0; memcpy(&res,pdu->Parameter,sizeof(res));
    if(res==false) QMessageBox::information(&Index::getInstance(),"Qt网盘","删除文件夹失败!");
    else
    {
        QMessageBox::information(&Index::getInstance(),"Qt网盘","删除文件夹成功");
        Index::getInstance().getPtrFile()->flushfile();
    }
}

void MsgHandler::FC_DELFILE(PDU *pdu)
{
    bool res=0; memcpy(&res,pdu->Parameter,sizeof(res));
    if(res==false) QMessageBox::information(&Index::getInstance(),"Qt网盘","删除文件失败!");
    else
    {
        QMessageBox::information(&Index::getInstance(),"Qt网盘","删除文件成功");
        Index::getInstance().getPtrFile()->flushfile();
    }
}

void MsgHandler::FC_RENAME(PDU *pdu)
{
    bool res=0; memcpy(&res,pdu->Parameter,sizeof(res));
    if(res==false) QMessageBox::information(&Index::getInstance(),"Qt网盘","重命名文件失败!");
    else
    {
        QMessageBox::information(&Index::getInstance(),"Qt网盘","重命名文件成功");
        Index::getInstance().getPtrFile()->flushfile();
    }
}

void MsgHandler::FC_MOVEFILE(PDU *pdu)
{
    bool res=0; memcpy(&res,pdu->Parameter,sizeof(res));
    if(res==false) QMessageBox::information(&Index::getInstance(),"Qt网盘","重命名文件失败!");
    else
    {
        QMessageBox::information(&Index::getInstance(),"Qt网盘","移动文件成功");
        Index::getInstance().getPtrFile()->flushfile();
    }
}

void MsgHandler::FC_UPLOADFILE(PDU *pdu)
{
    int res=0; memcpy(&res,pdu->Parameter,sizeof(res));
    if(res==-1) QMessageBox::information(&Index::getInstance(),"Qt网盘","已有文件正在上传");
    else if(res==0) QMessageBox::information(&Index::getInstance(),"Qt网盘","网盘创建文件失败");
    else if(res==1)
    {
        QMessageBox::information(&Index::getInstance(),"Qt网盘","上传文件开始");
        Index::getInstance().getPtrFile()->uploadfiledata();
    }
}

void MsgHandler::FC_UPLOADFILEDATA(PDU *pdu)
{
    bool res=0; memcpy(&res,pdu->Parameter,sizeof(res));
    if(res==false) QMessageBox::information(&Index::getInstance(),"Qt网盘","上传文件出错");
    else
    {
        QMessageBox::information(&Index::getInstance(),"Qt网盘","上传文件成功");
        Index::getInstance().getPtrFile()->flushfile();
    }
}

void MsgHandler::FC_DOWNLOADFILE(PDU *pdu)
{
    int res=0; memcpy(&res,pdu->Parameter,sizeof(res));
    qint64 filesize=0; memcpy(&filesize,pdu->Parameter+sizeof(res),sizeof(qint64));
    if(res==-1) QMessageBox::information(&Index::getInstance(),"Qt网盘","已有文件正在下载");
    else if(res==0) QMessageBox::information(&Index::getInstance(),"Qt网盘","网盘打开文件失败");
    else if(res==1)
    {
        QMessageBox::information(&Index::getInstance(),"Qt网盘","下载文件开始");
        Index::getInstance().getPtrFile()->downloadfile(filesize);
    }
}

void MsgHandler::FC_DOWNLOADFILEDATA(PDU *pdu)
{
    qDebug()<<"Downloading";
    Index::getInstance().getPtrFile()->downloadfiledata(pdu->MSG,pdu->uiMSgLen);
}

void MsgHandler::FC_SHAREFILE(PDU *pdu)
{
    QMessageBox::information(&Index::getInstance(),"Qt网盘","成功分享文件");
}

void MsgHandler::FC_SHAREFILEREC(PDU *pdu)
{
    //获取分享文件名
    QString FilePath=QString(pdu->MSG);
    QString FileName=FilePath.right(FilePath.size()-FilePath.lastIndexOf('/')-1);

    //询问用户是否接收文件
    int res=QMessageBox::question(&Index::getInstance(),"Qt网盘",QString("是否接收%1分享的文件%2").arg(pdu->Parameter).arg(FileName));

    if(res==QMessageBox::Yes)
    {
        PDU*respdu=mkPDU(FilePath.toStdString().size()+1);
        respdu->uiMSGType=ENUM_MSG_TYPE_SHAREFILEAGREE_REQUEST;
        memcpy(respdu->Parameter,Client::getInstance().getUserName().toStdString().c_str(),32);
        memcpy(respdu->MSG,FilePath.toStdString().c_str(),FilePath.toStdString().size());

        Client::getInstance().sendPDU(respdu);
    }
}

void MsgHandler::FC_SHAREFILERECRES(PDU *pdu)
{
    bool res=0; memcpy(&res,pdu->Parameter,sizeof(res));
    if(res==false) QMessageBox::information(&Index::getInstance(),"Qt网盘","接收文件失败");
    else
    {
        QMessageBox::information(&Index::getInstance(),"Qt网盘","接收文件成功");
        Index::getInstance().getPtrFile()->flushfile();
    }
}












#include "friend.h"
#include "client.h"
#include "protocol.h"
#include "ui_friend.h"

#include <QInputDialog>
#include <QtDebug>
#include <qmessagebox.h>

Friend::Friend(QWidget *parent) :QWidget(parent),ui(new Ui::Friend)
{
    ui->setupUi(this);
    ptr_OnlineUsers=new OnlineUsers();
    ptr_Chat=new Chat();
}

Friend::~Friend()
{
    delete ui;
    delete ptr_OnlineUsers;
    delete ptr_Chat;
}

OnlineUsers* Friend::getPtrOnlineUsers()
{
    return this->ptr_OnlineUsers;
}

Chat *Friend::getPtrChat()
{
    return this->ptr_Chat;
}

QListWidget *Friend::getPtrLW()
{
    return ui->lw_onlinefriend;
}

void Friend::on_bnt_finduser_clicked()
{
    bool isOK=false;
    QString name=QInputDialog::getText(this,"查找用户","搜索用户名:",QLineEdit::Normal,"",&isOK);

    //判断用户名是否为空
    if(name.isEmpty())
    {
        QMessageBox::information(this,"Qt网盘","用户名不能为空");
        return;
    }
    //判断用户名是否为自己
    if(name==Client::getInstance().getUserName())
    {
        QMessageBox::information(this,"Qt网盘","用户名不能为自己");
        return;
    }
    if(isOK==false) return;

    //创建协议数据单元并发送至服务器
    PDU*pdu=mkPDU(0);
    pdu->uiMSGType=ENUM_MSG_TYPE_FINDUSER_REQUEST;
    memcpy(pdu->Parameter,name.toStdString().c_str(),32);

    Client::getInstance().sendPDU(pdu);

}

void Friend::on_bnt_onlineusers_clicked()
{
    //显示在线用户界面(如果此时没有被显示的话)
    if(ptr_OnlineUsers->isHidden())
    {
        ptr_OnlineUsers->show();
    }
    //创建协议数据单元并发送至服务器
    PDU*pdu=mkPDU(0);
    pdu->uiMSGType=ENUM_MSG_TYPE_ONLINEUSERS_REQUEST;

    Client::getInstance().sendPDU(pdu);
}

void Friend::on_bnt_flushlist_clicked()
{
     Client::getInstance().FlushFriendList();
}

void Friend::UpdateFriendList(QStringList &obj)
{
    //每次更新前将好友列表之前的内容清空
    ui->lw_onlinefriend->clear();
    //更新好友列表
    ui->lw_onlinefriend->addItems(obj);
}

/*
 * 删除好友流程:
 * 客户端点击删除好友按钮->获取列表中选中元素文本内容->通过协议单元发送给服务端进行处理
 * 服务端收到消息交给数据库进行处理->数据库首先检查双方是否为好友->删除当前用户和目标用户的好友关系
 * 服务端完成相关操作后将结果反馈给客户端即可
*/
void Friend::on_bnt_delfriend_clicked()
{
    QListWidgetItem* item=ui->lw_onlinefriend->currentItem();

    //未选中好友
    if(item==nullptr)
    {
        QMessageBox::information(this,"Qt网盘","请选择你想删除的好友");
        return;
    }
    QString TarName=item->text();

    //确认是否删除好友
    int res=QMessageBox::question(this,"Qt网盘",QString("确认删除好友%1吗").arg(TarName));

    if(res==QMessageBox::Yes)
    {
        //创建协议数据单元并发送至服务器
        PDU*pdu=mkPDU(0);
        memcpy(pdu->Parameter,Client::getInstance().getUserName().toStdString().c_str(),32);
        memcpy(pdu->Parameter+32,TarName.toStdString().c_str(),32);

        pdu->uiMSGType=ENUM_MSG_TYPE_DELETEFRIEND_REQUEST;

        Client::getInstance().sendPDU(pdu);
    }

}

void Friend::on_bnt_chat_clicked()
{
    QListWidgetItem* item=ui->lw_onlinefriend->currentItem();

    //未选中好友
    if(item==nullptr)
    {
        QMessageBox::information(this,"Qt网盘","选择你想聊天的好友");
        return;
    }

    //获取目标好友名
    ptr_Chat->TarFriend=item->text();

    //显示聊天界面
    if(ptr_Chat->isHidden()) ptr_Chat->show();
}

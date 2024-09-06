#ifndef FRIEND_H
#define FRIEND_H

#include "chat.h"
#include "onlineusers.h"
#include <QWidget>

namespace Ui {
class Friend;
}

class Friend : public QWidget
{
    Q_OBJECT

public:
    explicit Friend(QWidget *parent = nullptr);
    ~Friend();

    //获取指向在线用户界面的指针
    OnlineUsers* getPtrOnlineUsers();
    //获取指向聊天界面的指针
    Chat* getPtrChat();
    //获取指向在线好友列表的指针
    QListWidget* getPtrLW();
    //好友列表中刷新好友(具体实现)
    void UpdateFriendList(QStringList& obj);

private slots:
    //好友界面查找用户(点击查找用户按钮)
    void on_bnt_finduser_clicked();
    //好友界面显示在线好友(点击在线好友按钮)
    void on_bnt_onlineusers_clicked();
    //好友界面刷新好友列表(点击刷新好友列表按钮)
    void on_bnt_flushlist_clicked();
    //好友界面删除好友(点击删除好友按钮)
    void on_bnt_delfriend_clicked();

    void on_bnt_chat_clicked();

private:
    Ui::Friend *ui;

    //指向在线好友界面的指针
    OnlineUsers* ptr_OnlineUsers;
    //指向在线好友界面的指针
    Chat* ptr_Chat;
};

#endif // FRIEND_H

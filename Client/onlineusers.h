#ifndef ONLINEUSERS_H
#define ONLINEUSERS_H

#include <QListWidget>
#include <QWidget>

namespace Ui {
class OnlineUsers;
}

class OnlineUsers : public QWidget
{
    Q_OBJECT

public:
    //更新在线用户列表框
    void updateOnlineUsersList(const QStringList& obj);

    explicit OnlineUsers(QWidget *parent = nullptr);
    ~OnlineUsers();

private slots:
    void on_LW_onlineusers_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::OnlineUsers *ui;
};

#endif // ONLINEUSERS_H

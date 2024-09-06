#ifndef CHAT_H
#define CHAT_H

#include <QWidget>

namespace Ui {
class Chat;
}

class Chat : public QWidget
{
    Q_OBJECT

public:
    explicit Chat(QWidget *parent = nullptr);
    ~Chat();

    //目标好友名
    QString TarFriend;

    //更新聊天内容
    void UpdateChatText(QString Text);
private slots:
    //向目标好友发送消息(点击发送按钮)
    void on_bnt_spend_clicked();
private:
    Ui::Chat *ui;
};

#endif // CHAT_H

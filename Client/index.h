#ifndef INDEX_H
#define INDEX_H

#include "friend.h"
#include "file.h"
#include <QWidget>

namespace Ui {
class Index;
}

class Index : public QWidget
{
    Q_OBJECT

public:
    ~Index();
    //获取指向好友界面的指针
    Friend* getPtrFriend();
    //获取指向文件界面的指针
    File* getPtrFile();
    //全局访问节点
   static Index& getInstance();
private slots:
    //切换至好友界面
    void on_bnt_friend_clicked();
    //切换至文件界面
    void on_bnt_file_clicked();

private:
    Ui::Index *ui;

    //设置单例模式
    explicit Index(QWidget *parent = nullptr);
    Index(const Index& other) = delete;
    Index& operator=(const Index& other) = delete;
};

#endif // INDEX_H

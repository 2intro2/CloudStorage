#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>

namespace Ui {
class ShareFile;
}

class ShareFile : public QWidget
{
    Q_OBJECT

public:
    explicit ShareFile(QWidget *parent = nullptr);
    ~ShareFile();
    //更新在线好友
    void updateonlinefriends();
private slots:
    //全选按钮
    void on_bnt_allSeleceted_clicked();
    //取消全选按钮
    void on_bnt_cancelAC_clicked();
    //确认按钮(点击分享)
    void on_bnt_confirm_clicked();

private:
    Ui::ShareFile *ui;
};

#endif // SHAREFILE_H

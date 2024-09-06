#include "client.h"
#include "index.h"
#include "ui_index.h"

Index::Index(QWidget *parent) :QWidget(parent),ui(new Ui::Index)
{
    ui->setupUi(this);

    this->resize(1280,960);
    this->setWindowIcon(QIcon(":/res/pic/yun.png"));
    this->setWindowTitle(QString("Qt网盘:")+Client::getInstance().getUserName());
    //刷新好友列表
    Client::getInstance().FlushFriendList();
}

Index& Index::getInstance()
{
    static Index instance;
    return instance;
}

Index::~Index()
{
    delete ui;
}

Friend *Index::getPtrFriend()
{
    return ui->page_friend;
}

File *Index::getPtrFile()
{
    return ui->page_file;
}

void Index::on_bnt_friend_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void Index::on_bnt_file_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

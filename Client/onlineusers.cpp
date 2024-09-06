#include "onlineusers.h"
#include "ui_onlineusers.h"
#include "client.h"

OnlineUsers::OnlineUsers(QWidget *parent) :QWidget(parent),ui(new Ui::OnlineUsers)
{
    ui->setupUi(this);

    this->setWindowTitle("Qt网盘");
    this->setWindowIcon(QIcon(":/res/pic/yun.png"));
}

OnlineUsers::~OnlineUsers()
{
    delete ui;
}

void OnlineUsers::on_LW_onlineusers_itemDoubleClicked(QListWidgetItem *item)
{
    //获取当前用户名和目标用户名
    QString strCurName=Client::getInstance().getUserName();
    QString strTarName=item->text();

    qDebug()<<"CurName:"<<strCurName<<"TarName:"<<strTarName;
    //创建pdu
    PDU* pdu=mkPDU(0);
    pdu->uiMSGType=ENUM_MSG_TYPE_SENDINVATION_REQUEST;
    memcpy(pdu->Parameter,strCurName.toStdString().c_str(),32);
    memcpy(pdu->Parameter+32,strTarName.toStdString().c_str(),32);

    Client::getInstance().sendPDU(pdu);

}

void OnlineUsers::updateOnlineUsersList(const QStringList &obj)
{
    //每次更新前将在线用户列表之前的内容清空
    ui->LW_onlineusers->clear();
    //更新在线用户列表
    ui->LW_onlineusers->addItems(obj);
}



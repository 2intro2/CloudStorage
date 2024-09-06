#include "chat.h"
#include "protocol.h"
#include "ui_chat.h"
#include "client.h"

Chat::Chat(QWidget *parent) :QWidget(parent),ui(new Ui::Chat)
{
    ui->setupUi(this);
    this->resize(1280,960);
    this->setWindowIcon(QIcon(":/res/pic/yun.png"));
    this->setWindowTitle(QString("聊天"));
}

Chat::~Chat()
{
    delete ui;
}

void Chat::on_bnt_spend_clicked()
{
    QString strMsg=ui->le_spendtext->text();
    ui->le_spendtext->clear();

    if(strMsg.size()==0) return;

    //客户端发送的消息也要在聊天框中显示
    ui->le_chattext->append(QString("%1: %2")
                            .arg(Client::getInstance().getUserName())
                            .arg(strMsg));

    //创建协议数据单元
    PDU* pdu=mkPDU(strMsg.toStdString().size());
    pdu->uiMSGType=ENUM_MSG_TYPE_CHAT_REQUEST;

    memcpy(pdu->Parameter,Client::getInstance().getUserName().toStdString().c_str(),32);
    memcpy(pdu->Parameter+32,TarFriend.toStdString().c_str(),32);
    memcpy(pdu->MSG,strMsg.toStdString().c_str(),strMsg.toStdString().size());

    Client::getInstance().sendPDU(pdu);
}

void Chat::UpdateChatText(QString Text)
{
    ui->le_chattext->append(Text);
}

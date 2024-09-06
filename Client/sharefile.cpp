#include "sharefile.h"
#include "client.h"
#include "index.h"
#include "ui_sharefile.h"

ShareFile::ShareFile(QWidget *parent) :QWidget(parent),ui(new Ui::ShareFile)
{
    ui->setupUi(this);
}

ShareFile::~ShareFile()
{
    delete ui;
}

void ShareFile::updateonlinefriends()
{
    ui->listWidget->clear();

    QListWidget* pLW=Index::getInstance().getPtrFriend()->getPtrLW();

    for(int i=0;i<pLW->count();i++)
    {
        ui->listWidget->addItem(new QListWidgetItem(*pLW->item(i)));
    }
}

void ShareFile::on_bnt_allSeleceted_clicked()
{
    for(int i=0;i<ui->listWidget->count();i++)
    {
        ui->listWidget->item(i)->setSelected(true);
    }
}

void ShareFile::on_bnt_cancelAC_clicked()
{
    for(int i=0;i<ui->listWidget->count();i++)
    {
        ui->listWidget->item(i)->setSelected(false);
    }
}

void ShareFile::on_bnt_confirm_clicked()
{
    //整合需要发给服务器的信息
    //用户名
    QString CurName=Client::getInstance().getUserName();
    //分享文件所在路径
    QString FilePath=Index::getInstance().getPtrFile()->getCurPath()+"/"+Index::getInstance().getPtrFile()->getShareFileName();
    //分享好友信息
    QList<QListWidgetItem*> LWItemList=ui->listWidget->selectedItems();
    int friendcnt=LWItemList.size();

    //打包到协议数据单元
    PDU* pdu=mkPDU(friendcnt*32+FilePath.toStdString().size()+1);
    pdu->uiMSGType=ENUM_MSG_TYPE_SHAREFILE_REQUEST;
    memcpy(pdu->Parameter,CurName.toStdString().c_str(),32);
    memcpy(pdu->Parameter+32,&friendcnt,sizeof(int));

    for(int i=0;i<friendcnt;i++)
    {
        memcpy(pdu->MSG+i*32,LWItemList[i]->text().toStdString().c_str(),32);
    }

    memcpy(pdu->MSG+friendcnt*32,FilePath.toStdString().c_str(),FilePath.toStdString().size());

    Client::getInstance().sendPDU(pdu);
}


















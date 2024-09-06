#include "file.h"
#include "ui_file.h"
#include "client.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

File::File(QWidget *parent) :QWidget(parent),ui(new Ui::File)
{
    ui->setupUi(this);

    //初始化分享界面
    ShareFileWindow=new ShareFile();

    //初始化两个路径
    UserPath=QString("./filesys/%1").arg(Client::getInstance().getUserName());
    CurPath=UserPath;

    //刷新文件
    flushfile();
    //初始化上传状态
    isUpload=false;
    //初始化下载状态
    isDownload=false;
}

File::~File()
{
    delete ui;
    delete ShareFileWindow;
}

QString File::getShareFileName()
{
    return this->ShareFileName;
}

QString File::getCurPath()
{
    return this->CurPath;
}

void File::on_bnt_mkdir_clicked()
{
    //获取要创建的文件夹名字
    QString NewDirName=QInputDialog::getText(this,"Qt网盘","新建文件夹名:");
    if(NewDirName.size()==0||NewDirName.size()>32)
    {
        QMessageBox::information(this,"Qt网盘","文件夹名长度非法!");
        return;
    }

    qDebug()<<"CN"<<CurPath<<"NN"<<NewDirName;

   //创建协议数据单元
   PDU*pdu=mkPDU(CurPath.toStdString().size()+1);
   pdu->uiMSGType=ENUM_MSG_TYPE_MKDIR_REQUEST;
   memcpy(pdu->Parameter,NewDirName.toStdString().c_str(),32);
   memcpy(pdu->MSG,CurPath.toStdString().c_str(),CurPath.toStdString().size());

   Client::getInstance().sendPDU(pdu);
}

void File::flushfile()
{
    //创建协议数据单元
    //向客户端发送当前路径(刷新当前路径的文件)
    PDU*pdu=mkPDU(CurPath.toStdString().size()+1);
    pdu->uiMSGType=ENUM_MSG_TYPE_FLUSHFILE_REQUEST;
    memcpy(pdu->MSG,CurPath.toStdString().c_str(),CurPath.toStdString().size());

    Client::getInstance().sendPDU(pdu);
}

void File::updatefilelist(QList<FileInfo *> pFileInfoList)
{
    //更新之前清除旧的文件信息 并释放空间
    foreach(FileInfo* pFileInfo,CurFileInfoList)
    {
        delete pFileInfo;
    }
    CurFileInfoList.clear();
    CurFileInfoList=pFileInfoList;
    ui->listWidget->clear();

    //开始刷新列表
    for(int i=0;i<CurFileInfoList.size();i++)
    {
        if(QString(CurFileInfoList[i]->FileName)=="."||QString(CurFileInfoList[i]->FileName)=="..") continue;

        QListWidgetItem* LWItem=new QListWidgetItem;

        LWItem->setText(CurFileInfoList[i]->FileName);
        if(CurFileInfoList[i]->FileType==0) LWItem->setIcon(QIcon(QPixmap(":/res/icon/mulu.png")));
        else if(CurFileInfoList[i]->FileType==1) LWItem->setIcon(QIcon(QPixmap(":/res/icon/wenjian.png")));

        qDebug()<<"FileName"<<CurFileInfoList[i]->FileName<<endl;

        ui->listWidget->addItem(LWItem);
    }

}

void File::uploadfiledata()
{
    //打开上传文件
    QFile file(UploadFilePath);
    if(file.open(QIODevice::ReadOnly)==false)
    {
        QMessageBox::warning(this,"Qt网盘","打开文件失败!");
        return;
    }

    //更新上传状态
    isUpload=true;

    //规定每次上传数据的大小(提高安全性)
    PDU* filedata=mkPDU(4096);
    filedata->uiMSGType=ENUM_MSG_TYPE_UPLOADFILEDATA_REQUEST;
    //循环上传文件数据
    while(true)
    {
        int res=file.read(filedata->MSG,4096);
        if(res==0) break;
        if(res<0)
        {
            QMessageBox::warning(this,"Qt网盘","读取文件内容失败");
            break;
        }

        //每一次并不一定能够读取4096字节的数据 所以要计算一下正确的消息长度和pdu长度
        filedata->uiMSgLen=res;
        filedata->uiPDULen=sizeof(PDU)+res;
        //将数据发送给服务器
        Client::getInstance().getTcpSocket().write((char*)filedata,filedata->uiPDULen);
    }

    //更新上传状态
    isUpload=false;
    //关闭文件 释放空间
    file.close();
    free(filedata);
    filedata=NULL;

}

void File::downloadfile(qint64 FileSize)
{
    //打开要写入下载数据的文件
    DownloadFile.setFileName(DownloadFilePath);

    if(FileSize==0)
    {
        DownloadFile.open(QIODevice::WriteOnly);
        DownloadFile.close();
        QMessageBox::information(this,"Qt网盘","下载文件完成");
        return;
    }

    //更新下载文件属性
    DownloadFileSize=FileSize;
    DownloadFileRec=0;
    isDownload=true;

    if(DownloadFile.open(QIODevice::WriteOnly)==false)
    {
        QMessageBox::warning(this,"Qt网盘","保存路径打开文件失败");
        return;
    }

    //创建协议数据单元
    PDU* pdu=mkPDU(0);
    pdu->uiMSGType=ENUM_MSG_TYPE_DOWNLOADFILEDATA_REQUEST;
    Client::getInstance().sendPDU(pdu);

}

void File::downloadfiledata(char *buf, qint64 size)
{
    //将数据写入本地文件中
    DownloadFile.write(buf,size);
    DownloadFileRec+=size;

    //数据没有完全下载就继续
    if(DownloadFileRec< DownloadFileSize) return;

    //完全下载更新下载状态
    isDownload=false;
    DownloadFile.close();
    QMessageBox::information(this,"Qt网盘","下载文件完成");
}

void File::on_bnt_flushfile_clicked()
{
    this->flushfile();
}

void File::on_bnt_deldir_clicked()
{
    QListWidgetItem* item=ui->listWidget->currentItem();

    //未选中文件夹
    if(item==nullptr)
    {
        QMessageBox::information(this,"Qt网盘","请选择你想删除的文件夹");
        return;
    }

    QString DirName=item->text();

    //若选中的是文件 提醒用户
    for(int i=0;i<CurFileInfoList.size();i++)
    {
        if(CurFileInfoList[i]->FileName==DirName)
        {
            if(CurFileInfoList[i]->FileType==1)
            {
               QMessageBox::information(this,"Qt网盘","请选择你想删除的文件夹而不是文件");
               return;
            }
            else break;
        }
    }

    //确认是否删除文件夹
    int res=QMessageBox::question(this,"Qt网盘",QString("确认删除文件夹%1吗").arg(DirName));
    if(res==QMessageBox::Yes)
    {
        //创建协议数据单元
        PDU*pdu=mkPDU(CurPath.toStdString().size()+1);
        pdu->uiMSGType=ENUM_MSG_TYPE_DELDIR_REQUEST;
        memcpy(pdu->Parameter,DirName.toStdString().c_str(),32);
        memcpy(pdu->MSG,CurPath.toStdString().c_str(),CurPath.toStdString().size());

        Client::getInstance().sendPDU(pdu);
    }
}

void File::on_bnt_delfile_clicked()
{
    QListWidgetItem* item=ui->listWidget->currentItem();

    //未选中文件
    if(item==nullptr)
    {
        QMessageBox::information(this,"Qt网盘","请选择你想删除的文件");
        return;
    }

    QString FileName=item->text();

    //若选中的是文件夹 提醒用户
    for(int i=0;i<CurFileInfoList.size();i++)
    {
        if(CurFileInfoList[i]->FileName==FileName)
        {
            if(CurFileInfoList[i]->FileType==0)
            {
               QMessageBox::information(this,"Qt网盘","请选择你想删除的文件而不是文件夹");
               return;
            }
            else break;
        }
    }

    //确认是否删除文件
    int res=QMessageBox::question(this,"Qt网盘",QString("确认删除文件%1吗").arg(FileName));
    if(res==QMessageBox::Yes)
    {
        //创建协议数据单元
        PDU*pdu=mkPDU(CurPath.toStdString().size()+1);
        pdu->uiMSGType=ENUM_MSG_TYPE_DELFILE_REQUEST;
        memcpy(pdu->Parameter,FileName.toStdString().c_str(),32);
        memcpy(pdu->MSG,CurPath.toStdString().c_str(),CurPath.toStdString().size());

        Client::getInstance().sendPDU(pdu);
    }
}

void File::on_bnt_rename_clicked()
{
    QListWidgetItem* item=ui->listWidget->currentItem();

    //未选中文件或文件夹
    if(item==nullptr)
    {
        QMessageBox::information(this,"Qt网盘","请选择需要重命名的文件");
        return;
    }

    QString OldName=item->text();

    //获取新名字并确认新名字是否合法
    QString NewName=QInputDialog::getText(this,"Qt网盘","新名字:");
    if(NewName.size()==0||NewName.size()>32)
    {

        QMessageBox::information(this,"Qt网盘","文件夹名长度非法!");
        return;
    }

    //创建协议数据单元
    PDU*pdu=mkPDU(CurPath.toStdString().size()+1);
    pdu->uiMSGType=ENUM_MSG_TYPE_RENAME_REQUEST;
    memcpy(pdu->Parameter,OldName.toStdString().c_str(),32);
    memcpy(pdu->Parameter+32,NewName.toStdString().c_str(),32);
    memcpy(pdu->MSG,CurPath.toStdString().c_str(),CurPath.toStdString().size());

    Client::getInstance().sendPDU(pdu);
}

void File::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{

    QString DirName=item->text();
    //判断选中目标是否为文件夹
    //若选中的是文件 则不做反应
    for(int i=0;i<CurFileInfoList.size();i++)
    {
        if(CurFileInfoList[i]->FileName==DirName)
        {
            if(CurFileInfoList[i]->FileType==1) return;
        }
    }

    //更改当前路径 并刷新文件列表
    CurPath=QString("%1/%2").arg(CurPath).arg(DirName);
    flushfile();
}

void File::on_bnt_backup_clicked()
{
    //如果当前位置为第一级目录 则提醒用户
    if(CurPath==UserPath)
    {
       QMessageBox::information(this,"Qt网盘","已在第一级目录");
       return;
    }

    //更改当前路径 并刷新文件列表
    int pos=CurPath.lastIndexOf('/');
    CurPath.truncate(pos);

    flushfile();
}

void File::on_bnt_movefile_clicked()
{
    if(ui->bnt_movefile->text()!=QString("确认/取消"))
    {
        QListWidgetItem* item=ui->listWidget->currentItem();

        //未选中文件或文件夹
        if(item==nullptr)
        {
            QMessageBox::information(this,"Qt网盘","请选择需要移动的文件");
            return;
        }

        //记录移动文件信息
        MoveFileName=item->text();
        MoveFilePath=QString("%1/%2").arg(CurPath).arg(MoveFileName);

        //改变按钮文字
        ui->bnt_movefile->setText("确认/取消");
        return;
    }

    QListWidgetItem* item=ui->listWidget->currentItem();
    QString TarPath,BoxMsg;

    //两种情况  选中某个文件夹或在某个文件夹内
    if(item!=nullptr)
    {
        QString DirName=item->text();
        //若选中的是文件 提醒用户
        for(int i=0;i<CurFileInfoList.size();i++)
        {
            if(CurFileInfoList[i]->FileName==DirName)
            {
                if(CurFileInfoList[i]->FileType==1)
                {
                   QMessageBox::information(this,"Qt网盘","请选中文件夹");
                   return;
                }
                else break;
            }
        }
        TarPath=QString("%1/%2/%3").arg(CurPath).arg(DirName).arg(MoveFileName);
        BoxMsg=QString("确认将文件移动到文件夹%1内吗?").arg(DirName);
    }
    else
    {
        TarPath=QString("%1/%2").arg(CurPath).arg(MoveFileName);
        BoxMsg=QString("确认将文件移动到当前文件夹内吗?");
    }

    //确认用户是否移动
    int res=QMessageBox::information(this,"Qt网盘",BoxMsg,"确认","取消");
    if(res==0)
    {
        //创建协议数据单元
        int sizeSrcPath=MoveFilePath.toStdString().size();
        int sizeTarPath=TarPath.toStdString().size();

        PDU*pdu=mkPDU(sizeSrcPath+sizeTarPath+1);
        pdu->uiMSGType=ENUM_MSG_TYPE_MOVEFILE_REQUEST;

        memcpy(pdu->Parameter,&sizeSrcPath,sizeof(int));
        memcpy(pdu->Parameter+sizeof(int),&sizeTarPath,sizeof(int));
        memcpy(pdu->MSG,MoveFilePath.toStdString().c_str(),sizeSrcPath);
        memcpy(pdu->MSG+sizeSrcPath,TarPath.toStdString().c_str(),sizeTarPath);

        Client::getInstance().sendPDU(pdu);
    }

    //改变按钮文字
    ui->bnt_movefile->setText("移动文件");
}

void File::on_bnt_uploadfile_clicked()
{
    //检查当前上传状态
    if(isUpload)
    {
        QMessageBox::warning(this,"Qt网盘","当前已有文件正在上传");
        return;
    }

    //获取文件在客户端中绝对路径并保存
    UploadFilePath.clear();
    UploadFilePath=QFileDialog::getOpenFileName();
    if(UploadFilePath.isEmpty()) return;

    //获取上传文件的文件名 文件大小和网盘的当前路径  封装到PDU中并发送给服务器
    QString UploadFileName=UploadFilePath.right(UploadFilePath.size()-UploadFilePath.lastIndexOf('/')-1);
    qDebug()<<"UploadFileName"<<UploadFileName;
    QFile file(UploadFilePath);
    qint64 FileSize=file.size();

    //创建协议数据单元

    PDU*pdu=mkPDU(CurPath.toStdString().size()+1);
    pdu->uiMSGType=ENUM_MSG_TYPE_UPLOADFILE_REQUEST;

    memcpy(pdu->Parameter,UploadFileName.toStdString().c_str(),32);
    memcpy(pdu->Parameter+32,&FileSize,sizeof(qint64));
    memcpy(pdu->MSG,CurPath.toStdString().c_str(),CurPath.toStdString().size());

    Client::getInstance().sendPDU(pdu);
}

void File::on_bnt_downloadfile_clicked()
{
    //检查当前下载状态
    if(isDownload)
    {
        QMessageBox::warning(this,"Qt网盘","当前正在下载文件");
        return;
    }

    //获取网盘中下载文件的信息
    QListWidgetItem* item=ui->listWidget->currentItem();

    //未选中文件
    if(item==nullptr)
    {
        QMessageBox::warning(this,"Qt网盘","请选择需要下载的文件");
        return;
    }

    //记录保存路径
    DownloadFilePath.clear();
    DownloadFilePath=QFileDialog::getSaveFileName();
    if(DownloadFilePath.isEmpty()) return;

    //创建协议数据单元(将想要下载的文件信息发送给服务器)
    QString FilePath=QString("%1/%2").arg(CurPath).arg(item->text());

    PDU*pdu=mkPDU(FilePath.toStdString().size()+1);
    pdu->uiMSGType=ENUM_MSG_TYPE_DOWNLOADFILE_REQUEST;

    memcpy(pdu->MSG,FilePath.toStdString().c_str(),FilePath.toStdString().size());

    Client::getInstance().sendPDU(pdu);
}

void File::on_bnt_sharefile_clicked()
{
    //判断是否选中文件
    QListWidgetItem* Item=ui->listWidget->currentItem();
    if(Item==NULL)
    {
        QMessageBox::information(this,"Qt网盘","请选择需要分享的文件");
        return;
    }

    //保存分享文件名
    ShareFileName=Item->text();

    //展示分享文件界面
    //更新分享文件界面的在线好友
    ShareFileWindow->updateonlinefriends();
    if(ShareFileWindow->isHidden())
    {
        ShareFileWindow->show();
    }
}


















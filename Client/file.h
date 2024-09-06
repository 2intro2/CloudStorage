#ifndef FILE_H
#define FILE_H

#include "protocol.h"
#include "sharefile.h"

#include <QFile>
#include <QWidget>
#include <qlistwidget.h>

namespace Ui {
class File;
}

class File : public QWidget
{
    Q_OBJECT

public:
    explicit File(QWidget *parent = nullptr);
    ~File();
    //获取分享文件名
    QString getShareFileName();
    //获取当前路径
    QString getCurPath();

    //刷新文件
    //封装刷新文件函数
    void flushfile();
    //刷新文件列表
    void updatefilelist(QList<FileInfo*> pFileInfoList);
    //上传文件数据
    void uploadfiledata();
    //下载文件数据
    void downloadfile(qint64 FileSize);
    void downloadfiledata(char* buf,qint64 size);

private slots:

    //在当前路径下新创建一个文件夹(点击创建文件夹按钮)
    void on_bnt_mkdir_clicked();
    //刷新文件(点击刷新文件按钮)
    void on_bnt_flushfile_clicked();
    //删除文件夹(点击删除文件夹按钮)
    void on_bnt_deldir_clicked();
    //删除文件(点击删除文件按钮)
    void on_bnt_delfile_clicked();
    //重命名文件(点击重命名文件按钮)
    void on_bnt_rename_clicked();
    //进入文件夹(双击某一个文件夹)
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    //返回上一级(点击上一级按钮)
    void on_bnt_backup_clicked();
    //移动文件(点击移动文件按钮)
    void on_bnt_movefile_clicked();
    //上传文件(点击上传文件按钮)
    void on_bnt_uploadfile_clicked();
    //下载文件(点击下载文件按钮)
    void on_bnt_downloadfile_clicked();
    //分享文件(点击分享文件按钮)
    void on_bnt_sharefile_clicked();

private:
    Ui::File *ui;

    //当前路径和用户的初始路径
    QString UserPath;
    QString CurPath;

    //保存当前路径下所有的文件信息
    QList<FileInfo*> CurFileInfoList;

    //移动文件时需要的临时变量
    QString MoveFileName;
    QString MoveFilePath;

    //上传文件时需要的临时变量
    QString UploadFilePath;
    bool isUpload;

    //下载文件时需要的临时变量
    QString DownloadFilePath;
    QFile DownloadFile;
    qint64 DownloadFileSize;
    qint64 DownloadFileRec;
    bool isDownload;

    //分享文件界面
    ShareFile* ShareFileWindow;
    //分享文件时需要的临时变量
    QString ShareFileName;
};

#endif // FILE_H

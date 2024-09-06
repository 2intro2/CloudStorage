#ifndef OPERATORDB_H
#define OPERATORDB_H

#include <QObject>
#include <QSqlDatabase>

class OperatorDB : public QObject
{
    Q_OBJECT
public:
    //提供全局访问节点
    static OperatorDB& getInstance();
    //连接数据库
    void connectDB();
    //数据库处理注册请求
    bool handleRegist(char* Name,char* PWD);
    //数据库处理登录请求
    bool handleLogin(char* Name,char* PWD);
    //数据库处理下线操作
    void handleOFFLine(const char* Name);
    //数据库查找用户
    int handleFindUser(char* Name);
    //数据库查找所有在线用户
    void handleFindOnlineUsers(QStringList& obj);
    //数据库判断好友邀请是否合法
    int handleFriendInvation(char* CurName,char* TarName);
    //数据库判断好友邀请是否合法
    void handleAddFriendship(char* CurName,char* TarName);
    //数据库查找某个用户的所有好友
    void handleFindFriends(char* CurName,QStringList& obj);
    //数据库删除用户的某个好友
    int handleDeleteFriendship(char* CurName,char* TarName);

    ~OperatorDB();
private:
    //创建数据库对象用于操作数据库(类似操作文件)
    QSqlDatabase My_DB;
    //设置单例模式
    explicit OperatorDB(QObject *parent = nullptr);
    OperatorDB(const OperatorDB& other) = delete;
    OperatorDB& operator=(const OperatorDB& other) = delete;
};

#endif // OPERATORDB_H

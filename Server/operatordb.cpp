#include "operatordb.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>

OperatorDB::OperatorDB(QObject *parent) : QObject(parent)
{
    //向数据库对象添加驱动
    My_DB=QSqlDatabase::addDatabase("QMYSQL");

}

void OperatorDB::connectDB()
{
    My_DB.setHostName("localhost");
    My_DB.setPort(3306);
    My_DB.setUserName("root");
    My_DB.setPassword("123456");
    My_DB.setDatabaseName("Qtstorage");
    if(My_DB.open()) qDebug()<<"数据库连接成功";
    else qDebug()<<"数据库连接失败"<<My_DB.lastError().text();
}

/*
 * query.next()用于将指针移动到查询结果集中的下一条记录
 * 每次调用query.next() 查询结果集中的指针会移动到下一条记录
 * 如果成功移动到下一条记录 query.next()会返回true
 * 如果指针已经指向查询结果集的末尾 或者查询结果为空 query.next()会返回false
*/
bool OperatorDB::handleRegist(char *Name, char *PWD)
{
    if(Name==NULL||PWD==NULL) return false;
    //创建sql语句对象用于在数据库中执行sql语句
    QSqlQuery query;
    //查询数据库中是否有用户名相同的记录
    QString sql=QString("select * from user_ifo where name='%1'").arg(Name);

    if(query.exec(sql)==false) return false; //查询失败
    if(query.next()) return false; //查询成功且有相关记录

    //向数据库中插入新用户信息
    sql=QString("insert into user_ifo(name,pwd) values('%1','%2')").arg(Name).arg(PWD);

    return query.exec(sql); //返回值为插入操作是否成功
}

bool OperatorDB::handleLogin(char *Name, char *PWD)
{
    if(Name==NULL||PWD==NULL) return false;
    //创建sql语句对象用于在数据库中执行sql语句
    QSqlQuery query;
    //查询数据库中是否存在该用户
    QString sql=QString("select * from user_ifo where name='%1' and pwd='%2'").arg(Name).arg(PWD);

    if(query.exec(sql)==false) return false; //查询失败
    if(query.next()==false) return false; //查询成功但没有相关记录(用户不存在)

    //用户存在 更新登录状态(上线)
    sql=QString("update user_ifo set online=1 where name='%1'").arg(Name);
    return query.exec(sql); //返回值为登录状态是否更新成功
}

void OperatorDB::handleOFFLine(const char *Name)
{
    if(Name==NULL) return;
    //创建sql语句对象用于在数据库中执行sql语句
    QSqlQuery query;
    //更新用户登录状态(下线)
    QString sql=QString("update user_ifo set online=0 where name='%1'").arg(Name);
    query.exec(sql);
    return;
}

int OperatorDB::handleFindUser(char *Name)
{
    //用户的三种状态 -1(用户不存在) 0(用户离线) 1(用户在线)
    if(Name==NULL) return -1;
    //创建sql语句对象用于在数据库中执行sql语句
    QSqlQuery query;
    //查询数据库中是否存在该用户
    QString sql=QString("select * from user_ifo where name='%1'").arg(Name);

    if(query.exec(sql)==false) return -1; //查询失败
    if(query.next()==false) return -1; //查询成功但没有相关记录(用户不存在)

    //用户存在 返回用户在线状态
    sql=QString("select online from user_ifo where name='%1'").arg(Name);
    if(query.exec(sql)==false) return -1; //查询失败
    query.next(); //移动到当前查询结果(由上可知查询结果一定存在)
    return query.value(0).toInt();; //返回值为用户在线状态
}

void OperatorDB::handleFindOnlineUsers(QStringList &obj)
{
    //创建sql语句对象用于在数据库中执行sql语句
    QSqlQuery query;
    //查询数据库中是否存在该用户
    QString sql=QString("select name from user_ifo where online=1");

    if(query.exec(sql)==false) return; //查询失败

    //如果存在在线用户 使用QStringList存储在线用户的用户名
    while(query.next()==true)
    {
        obj.append(query.value(0).toString());
    }
    return;
}

int OperatorDB::handleFriendInvation(char *CurName, char *TarName)
{
    if(CurName==NULL||TarName==NULL) return -1;
    /*
     * 好友邀请是否合法:
     * 1) 用户双方是否已经为好友
     * 2) 目标用户是否在线
     * -2 查询失败 -1 双方互为好友 0 双方不为好友但用户不在线 1 用户双方不为好友且用户在线(好友邀请合法)
    */
    //查询数据库中用户双方是否为好友
    QSqlQuery query;
    QString sql=QString(R"(select * from friend
                  where
                  (
                  user_id=(select id from user_ifo where name='%1')
                  and
                  friend_id=(select id from user_ifo where name='%2')
                  )
                  or
                  (
                  user_id=(select id from user_ifo where name='%3')
                  and
                  friend_id=(select id from user_ifo where name='%4')
                  ))").arg(CurName).arg(TarName).arg(TarName).arg(CurName);
    if(query.exec(sql)==false) return -2; //查询失败
    if(query.next()==true) return -1; //查询成功且存在相关记录(用户双方为好友)

    //用户双方不为好友 查询数据库中目标用户是否在线
    sql=QString("select * from user_ifo where name='%1' and online=1").arg(TarName);

    if(query.exec(sql)==false) return -2; //查询失败
    if(query.next()==false) return 0; //查询成功但没有相关记录(用户不在线)
    return 1; //用户双方不为好友且用户在线(好友邀请合法)
}

void OperatorDB::handleAddFriendship(char *CurName, char *TarName)
{
    if(CurName==NULL||TarName==NULL) return;
    QSqlQuery query;
    QString sql=QString(R"(insert into friend(user_id,friend_id)
                        select u1.id,u2.id from user_ifo u1
                        join user_ifo u2
                        on u1.name="%1" and u2.name="%2";)").arg(CurName).arg(TarName);
    query.exec(sql); //添加好友
    return;
}

void OperatorDB::handleFindFriends(char *CurName,QStringList &obj)
{
    if(CurName==NULL) return;
    //创建sql语句对象用于在数据库中执行sql语句
    QSqlQuery query;
    //查询该用户的所有好友
    QString sql=QString(R"(select name from user_ifo where
                        id in
                        (
                        select friend_id from friend where user_id=(select id from user_ifo where name='%1')
                        union
                        select user_id from friend where friend_id=(select id from user_ifo where name='%1')
                        )
                        and online=1;)")
                        .arg(CurName);
    query.exec(sql);
    while(query.next()==true)
    {
        obj.append(query.value(0).toString());
    }
    return;
}

int OperatorDB::handleDeleteFriendship(char *CurName, char *TarName)
{
    // 1 删除成功 0 删除失败(对方已经不是好友) -1 其他原因
    if(CurName==NULL||TarName==NULL) return -1;
    //首先查询数据库中用户双方是否为好友
    QSqlQuery query;
    QString sql=QString(R"(select * from friend
                  where
                  (
                  user_id=(select id from user_ifo where name='%1')
                  and
                  friend_id=(select id from user_ifo where name='%2')
                  )
                  or
                  (
                  user_id=(select id from user_ifo where name='%3')
                  and
                  friend_id=(select id from user_ifo where name='%4')
                  ))").arg(CurName).arg(TarName).arg(TarName).arg(CurName);
    if(query.exec(sql)==false) return -1;
    if(query.next()==false) return 0;
    //删除好友关系
    sql=QString(R"(delete from friend
                 where
                 (
                 user_id=(select id from user_ifo where name='%1')
                 and
                 friend_id=(select id from user_ifo where name='%2')
                 )
                 or
                 (
                 user_id=(select id from user_ifo where name='%3')
                 and
                 friend_id=(select id from user_ifo where name='%4')
                 );
                 )").arg(CurName).arg(TarName).arg(TarName).arg(CurName);
    if(query.exec(sql)==false) return -1;
    return 1; //删除好友成功
}

OperatorDB& OperatorDB::getInstance()
{
    static OperatorDB Instance;
    return Instance;
}

OperatorDB::~OperatorDB()
{
    //关闭数据库对象
    My_DB.close();
}

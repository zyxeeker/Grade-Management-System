//
// Created by 19047 on 2021/2/17.
//

#ifndef WEBSEVER_SQL_H
#define WEBSEVER_SQL_H

#include <string>
#include <iostream>
#include <mysql/mysql.h>
#include <list>

namespace sql{
    class SQLConnectionPool {
    public:
//        SQLConnectionPool();
        // 数据库初始化
        void init(std::string host, std::string user, std::string passwd, std::string database, int port = 3306);
        // 获取可用的连接数
        MYSQL * getAvailConn();
        // 连接数据库
        void initConnPool();
        // 释放连接
        bool releaseConnection(MYSQL *conn);

        void SearchTest(std::string str1);


    private:
        // 最大连接数
        int m_maxConn = 10;
        // 当前连接数
        int m_curConn = 0;
        // 连接池
        std::list<MYSQL *> m_connPool;

        MYSQL m_conn;
        std::string m_host;
        std::string m_user;
        std::string m_passwd;
        std::string m_database;
        int m_port;
    };

}
#endif //WEBSEVER_SQL_H

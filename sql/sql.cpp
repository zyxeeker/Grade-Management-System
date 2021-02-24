//
// Created by 19047 on 2021/2/17.
//

//
// Created by 19047 on 2021/2/17.
//

#include "sql.h"
#include <mysql/mysql.h>
#include <iostream>
#include "../log/log.h"
#include <list>
#include <thread>


namespace sql{
    MYSQL * SQLConnectionPool::getAvailConn() {
        MYSQL *conn = m_connPool.front();
        m_curConn += 1;
        return conn;
    }

//    bool SQLConnectionPool::releaseConnection() {
//        for (auto it : m_connPool){
//            MYSQL con = *it;
//            mysql_close(&con);
//            std::cout << "DONE"<<std::endl;
//        }
//    }

    bool SQLConnectionPool::releaseConnection(MYSQL *conn) {
        if (conn == NULL)
            return false;
    }

    void SQLConnectionPool::initConnPool() {
//        if (!SQLConnectionPool::getConnection()) {
//            std::thread::id threadId = std::this_thread::get_id();
//            Logger::Log(Logger::LogLevel::Level::FATAL, threadId, "MYSQL`S CONNECTIONS ALREADY HAD MAX NUMBER!");
//        }
//        else {
            for (int  i = 0; i < m_maxConn; ++i) {
                mysql_init(&m_conn);
                MYSQL *statue = NULL;
                std::string database = "test";
                statue = mysql_real_connect(&m_conn, m_host.c_str(), m_user.c_str(), m_passwd.c_str(), m_database.c_str(), m_port , NULL, 0);
                std::thread::id threadId = std::this_thread::get_id();
                if (statue == NULL) {
                    Logger::Log(Logger::LogLevel::Level::FATAL, threadId, "MYSQL`S CONNECTION CREATED FAILED!");
                }
                else {
                    m_connPool.push_back(&m_conn);
                    ++m_curConn;
                    Logger::Log(Logger::LogLevel::Level::INFO, threadId, "MYSQL`S CONNECTION CREATED!");
                }
//            }
        }
    }


    void SQLConnectionPool::init(std::string host, std::string user, std::string passwd, std::string database, int port) {
        m_host = host;
        m_user = user;
        m_passwd = passwd;
        m_port = port;
        m_database = database;
        // 创建连接
        SQLConnectionPool::initConnPool();

    }
}
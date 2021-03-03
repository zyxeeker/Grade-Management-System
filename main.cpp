#include <iostream>
#include "sql/sql.h"
#include <mysql/mysql.h>
#include <map>
#include "http/http.h"
#include "conn/conn.h"
#include <fstream>
#include <errno.h>
#include <cstring>
#include <iterator>
#include <codecvt>
#include <locale>

#include <sys/types.h>
#include <dirent.h>
#include <node/parse.h>
#include <yaml.h>
#include <vector>

MYSQL_RES *res;

const std::string SERVER_CONF = "/root/sync/webServer/config.yaml";

int main() {
    std::map<std::string, std::string> users;

    sql::SQLConnectionPool test;
    test.init("localhost", "root", "","test");
    MYSQL *conn = test.getAvailConn();
//    const char *str1 = "select * from user";
//    mysql_query(conn, str1);
    mysql_query(conn, "SELECT username,passwd FROM user");
    //从表中检索完整的结果集
    MYSQL_RES *result = mysql_store_result(conn);
    //返回结果集中的列数
    int num_fields = mysql_num_fields(result);

    //返回所有字段结构的数组
    MYSQL_FIELD *fields = mysql_fetch_fields(result);

    //从结果集中获取下一行，将对应的用户名和密码，存入map中
    while (MYSQL_ROW row = mysql_fetch_row(result))
    {
        std::string temp1(row[0]);
        std::string temp2(row[1]);
        users[temp1] = temp2;
    }
    std::cout << users["name"];


////

    YAML::Node config = YAML::LoadFile(SERVER_CONF);
    int port = config["port"].as<int>();
    int max_connections = config["max_connections"].as<int>();
    http_conn::conn test;
    test.init(max_connections, port);

    return 0;
}

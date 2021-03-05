//
// Created by ZYXeeker on 2021/3/5.
//

#ifndef WEBSEVER_SERVER_H
#define WEBSEVER_SERVER_H

#include <iostream>
#include <string>
#include <map>
#include <queue>

namespace server {
    class server_init {
    public:
        // 解析配置文件
        bool config_read(std::string SERVER_CONF = "/root/sync/webServer/config.yaml");

        // 读取文件
        bool file_read();
//        // 返回文件
//        std::map<std::string, std::string> get_files();

    public:
        static int m_port;
        static int m_max_connections;
        static std::map<std::string, std::string> m_files;

    private:
        std::queue<std::string> m_dir;

//        static std::map<std::string, std::string> m_files;



    };
}

#endif //WEBSEVER_SERVER_H

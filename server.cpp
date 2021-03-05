//
// Created by ZYXeeker on 2021/3/5.
//

#include <node/parse.h>
#include <yaml.h>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include "server.h"

namespace server {
    int server_init::m_port = 0;
    int server_init::m_max_connections = 0;

    std::map<std::string, std::string> server_init::m_files = std::map<std::string, std::string>();

    // 读取配置文件
    bool server_init::config_read(std::string SERVER_CONF) {
        YAML::Node config = YAML::LoadFile(SERVER_CONF);
        m_port = config["port"].as<int>();
        m_max_connections = config["max_connections"].as<int>();
    }

    // 读取文件
    bool server_init::file_read() {
        std::string path = "/root/sync/webServer/root";
        std::string folder_path = "/";
        std::string path_tmp;
        std::string path_cur;
        DIR *p_dir;
        struct dirent *ptr;

        m_dir.push(folder_path);

        while (!m_dir.empty()) {
            path_tmp = m_dir.front();
            path_cur = path + path_tmp;
            if (!(p_dir = opendir(path_cur.c_str())))
                std::cout << std::strerror(errno) << std::endl;
            m_dir.pop();
            while ((ptr = readdir(p_dir)) != 0) {
                if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0) {
                    std::string file_name = ptr->d_name;
                    if (ptr->d_type == DT_DIR) {
                        std::cout << ptr->d_name << std::endl;
                        m_dir.push("/" + file_name + "/");
                        continue;
                    }
                    std::string file_path = path_cur + file_name;
                    std::ifstream file(file_path);

                    if (!file.is_open())
                        std::cout << std::strerror(errno) << std::endl;

                    std::istreambuf_iterator<char> begin(file);
                    std::istreambuf_iterator<char> end;
                    std::string some_str(begin, end);

                    m_files[path_tmp + file_name] = some_str;
                }
            }
            closedir(p_dir);
        }
    }

//    // 返回文件
//    std::map<std::string, std::string> server_init::get_files() {
//        return m_files;
//    }
}
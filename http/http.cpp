//
// Created by ZYXeeker on 2021/2/24.
//
#include <strings.h>
#include "http.h"
#include <errno.h>
#include <dirent.h>
#include <cstring>
#include <fstream>

namespace http {
    // 解析报文请求
    HTTP_PACKET::PARSE_CODE Http::parse_method(std::string text) {
        int m = 0;

        try {
            if (!text.compare(0, 4, "POST", 0, 4)) {
                m_method = HTTP_PACKET::HTTP_METHOD::POST;
                text = text.substr(5);
            }
            if (!text.compare(0, 4, "GET ", 0, 4)) {
                m_method = HTTP_PACKET::HTTP_METHOD::GET;
                text = text.substr(4);
            }

            for (auto i : text) {
                if (i == ' ') {
                    m_url = text.substr(0, m);
                    text = text.substr(m + 1);
                }
                ++m;
            }

            m = 0;
            for (auto i : text) {
                if (i == '/') {
                    m_protocol = text.substr(0, m);
                    m_version = text.substr(m + 1);
                }
                ++m;
            }
        } catch (const char *msg) {
            return HTTP_PACKET::PARSE_ERROR;
        }

        return HTTP_PACKET::PARSE_DONE;
    }

    // 解析报文报头
    HTTP_PACKET::PARSE_CODE Http::parse_header(std::string text) {
        int m = 0;

        if (text.compare(0, 7, "Cookie:", 0, 7)) {
            for (auto i : text) {
                if (i == ':') {
                    if (text[m + 1] == ' ')
                        ++m;
                    std::string key = text.substr(0, m - 1);
                    std::string data = text.substr(m + 1);
                    m_headers[key] = data;

                    return HTTP_PACKET::PARSE_DONE;
                }
                ++m;
            }
        }

        return HTTP_PACKET::PARSE_ERROR;
    }

    // 解析报文内容
    HTTP_PACKET::PARSE_CODE Http::parse_content(std::string text) {
        int begin_index = 0;
        int data_begin_index = 0;
        std::string key;
        std::string data;

        try {
            for (int i = 0; i < text.length(); ++i) {
                if (text[i] == '=') {
                    key = text.substr(begin_index, i - begin_index);
                    data_begin_index = i + 1;
                }
                if (text[i] == '&') {
                    begin_index = i + 1;
                    if (begin_index > data_begin_index)
                        data = text.substr(data_begin_index, begin_index - data_begin_index - 1);
                    m_requests[key] = data;
                }
                if (i == text.length() - 1) {
                    data = text.substr(data_begin_index);
                    m_requests[key] = data;
                }
            }
        } catch (const char *msg) {
            return HTTP_PACKET::PARSE_ERROR;
        }

        return HTTP_PACKET::PARSE_DONE;

    }

    // 解析报文
    void Http::parse_packet(char *packet) {
        int i;
        int begin_index = 0;
        std::string text;

        std::string str1 = packet;

        // line
        for (i = 0; i < str1.length(); ++i) {
            if (str1[i] == '\r') {
                text = str1.substr(0, i);
                str1 = str1.substr(i + 2);
                if (Http::parse_method(text) == HTTP_PACKET::PARSE_CODE::PARSE_ERROR) {
                    std::thread::id threadId = std::this_thread::get_id();
                    Logger::Log(Logger::LogLevel::ERROR, threadId, "METHOD BAD PARSE!");
                }

                break;
            }
        }

        // header
        for (i = 0; i < str1.length(); ++i) {
            if (str1[i] == '\r') {
                if (str1[i + 2] == '\r') {
                    str1 = str1.substr(i + 3);
                    break;
                }

                text = str1.substr(begin_index, i - begin_index);
                begin_index = i + 2;
                i = i + 2;
                if (Http::parse_header(text) == HTTP_PACKET::PARSE_CODE::PARSE_ERROR) {
                    std::thread::id threadId = std::this_thread::get_id();
                    Logger::Log(Logger::LogLevel::ERROR, threadId, "HEADER BAD PARSE!");
                }
            }
        }

        // body
        if (str1.length() == 0)
            std::cout << "NULL";
        else if (Http::parse_content(text) == HTTP_PACKET::PARSE_CODE::PARSE_ERROR) {
            std::thread::id threadId = std::this_thread::get_id();
            Logger::Log(Logger::LogLevel::ERROR, threadId, "CONTENT BAD PARSE!");
        }
    }

    // 读取文件
    std::map<std::string, std::string> Http::open_file(std::string folder_path) {
        std::map<std::string, std::string> file_list;
        std::string path = "/root/sync/webServer/root" + folder_path;

        DIR *p_dir;
        struct dirent *ptr;
        if (!(p_dir = opendir(path.c_str())))
            std::cout << std::strerror(errno) << std::endl;
        while ((ptr = readdir(p_dir)) != 0) {

            if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0) {
                std::string file_name = ptr->d_name;
                if (file_name == "images" || file_name == "css" || file_name == "js")
                    continue;
                std::string file_path = path + file_name;
                std::ifstream file(file_path);

                if (!file.is_open())
                    std::cout << std::strerror(errno) << std::endl;

                std::istreambuf_iterator<char> begin(file);
                std::istreambuf_iterator<char> end;
                std::string some_str(begin, end);

                file_list[folder_path + file_name] = some_str;
//                std::cout << folder_path + file_name <<std::endl;
//                std::cout << file_list[folder_path + file_name] <<std::endl;
            }

        }
        closedir(p_dir);
        return file_list;

    }

    bool Http::init() {
        m_html = open_file("/");
        m_css = open_file("/css/");
//        m_js = open_file("/js/");
        m_image = open_file("/images/");
    }

    // 返回请求
    std::string Http::do_request() {
        std::string header = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/html; charset=UTF-8\r\n"
                             "\r\n";
        if (m_method == HTTP_PACKET::GET) {
            if (!m_url.compare(0, 5, "/css/", 0, 5)) {
                header = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: text/css\r\n"
                         "\r\n";
                header += m_css[m_url];
                return header;
            }
            if (!m_url.compare(0, 8, "/images/", 0, 8)) {
                header = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: images/gif\r\n"
                         "\r\n";
                header += m_image[m_url];
                return header;
            }
            if (!m_url.compare(0, 5, "/js/", 0, 5)) {
                header += m_js[m_url];
                std::cout << header << std::endl;
                return header;
            }
            if (m_url == "/") {
                header += m_html["/404.html"];

                return header;
            }
//            if (!m_url.compare(m_url.length()-4, 3, "htm", 0, 3)) {
//                header += m_html[m_url];
//                return header;
//            }


        }

    }


}

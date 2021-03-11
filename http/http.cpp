//
// Created by ZYXeeker on 2021/2/24.
//
#include <strings.h>
#include "http.h"
#include <errno.h>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include "../conn/conn.h"

const std::string OK = " 200 OK\r\n";
const std::string BAD_REQUEST = " 400 Bad Request\r\n";
const std::string MOVED_PERMANENTLY = " 301 Moved Permanently\r\n";
const std::string NOT_FOUND = " 404 Not Found\r\n";


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
    void Http::parse_packet(std::string str1, int client_num) {
        int i;
        int begin_index = 0;
        std::string text;
        m_client_num = client_num;

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

    // 返回请求
    void Http::do_request() {
        std::string header;
        std::string type;

        if (m_method == HTTP_PACKET::GET) {
            try {
                std::string style = ".css";
                std::string html = ".htm";
                std::string image = ".jpg";
                std::string js = ".js";

                if (m_url.find(style) != std::string::npos)
                    type = "Content-Type: text/css\r\n";
                if (m_url.find(html) != std::string::npos)
                    type = "Content-Type: text/html\r\n";
                if (m_url.find(image) != std::string::npos)
                    type = "Content-Type: image/*\r\n";
                if (m_url.find(js) != std::string::npos)
                    type = "Content-Type: application/javascript\r\n";
                header = "HTTP/1.1" + OK + type + "\r\n";
                header += server::server_init::m_files[m_url];
//                std::cout << header << std::endl;
//                std::cout << m_url << std::endl;
//                std::cout << server::server_init::m_files[m_url] << std::endl;
                http_conn::conn::send_packet(m_client_num, header);
//                }

            } catch (...) {
                header = "HTTP/1.1" + BAD_REQUEST;
            }
        }
    }
}


//
// Created by ZYXeeker on 2021/2/24.
//
#include <strings.h>
#include "http.h"

namespace http {

    // 从状态机
    HTTP_PACKET::HTTP_CODE HTTP_STATE_MACHINE::LINE_STATE(std::string str) {

    }

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
                    std::string key = text.substr(0, m);
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
    void Http::read_packet(char *packet) {
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
                    str1 = str1.substr(i + 4);
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

}

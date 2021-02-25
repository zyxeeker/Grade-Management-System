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
    HTTP_PACKET::LINE_CODE Http::parse_method(std::string text) {
        int m = 0;

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

    }

    // 解析报文报头
    HTTP_PACKET::LINE_CODE Http::parse_header(std::string text) {
        int m = 0;

        if (text.compare(0, 7, "Cookie:", 0, 7)) {
            for (auto i : text) {
                if (i == ':') {
                    if (text[m + 1] == ' ')
                        ++m;
                    std::string key = text.substr(0, m);
                    std::string data = text.substr(m + 1);
                    m_headers[key] = data;

                    return HTTP_PACKET::LINE_OK;
                }
                ++m;
            }
        }

        return HTTP_PACKET::LINE_ERROR;
    }

    // 解析报文内容
    HTTP_PACKET::LINE_CODE Http::parse_content(std::string text) {

    }


    // 主状态机
    HTTP_PACKET::HTTP_CODE Http::MAIN_STATE(std::string str) {

        switch (m_packet_state) {
            case HTTP_STATE_MACHINE::PACKET_STATE::CHECK_METHOD: {
                Http::parse_method(str);
                break;
            }
            case HTTP_STATE_MACHINE::PACKET_STATE::CHECK_HEADER: {
                Http::parse_header(str);
                break;
            }
            case HTTP_STATE_MACHINE::PACKET_STATE::CHECK_CONTENT: {
                Http::parse_content(str);
                break;
            }


        }
    }

    bool Http::put_packet_state(HTTP_STATE_MACHINE::PACKET_STATE pState) {
        m_packet_state = pState;
    }


    void Http::read_packet(char *packet) {
        int i;
        int begin_index = 0;
        int data_begin_index = 0;
        std::string text;
        std::string key;
        std::string data;

        std::string str1 = packet;

        // line
        for (i = 0; i < str1.length(); ++i) {
            if (str1[i] == '\r') {
                text = str1.substr(0, i);
                str1 = str1.substr(i + 2);
                Http::put_packet_state(HTTP_STATE_MACHINE::PACKET_STATE::CHECK_METHOD);
                Http::MAIN_STATE(text);
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
                Http::put_packet_state(HTTP_STATE_MACHINE::PACKET_STATE::CHECK_HEADER);
                Http::MAIN_STATE(text);
            }
        }

        // body
        if (str1.length() == 0) {
            std::cout << "NULL";
        } else {
            begin_index = 01;
            for (i = 0; i < str1.length(); ++i) {
                if (str1[i] == '=') {
                    key = str1.substr(begin_index, i - begin_index);
                    data_begin_index = i + 1;
                }
                if (str1[i] == '&') {
                    begin_index = i + 1;
                    if (begin_index > data_begin_index)
                        data = str1.substr(data_begin_index, begin_index - data_begin_index - 1);
                    m_requests[key] = data;
                }
                if (i == str1.length() - 1) {
                    data = str1.substr(data_begin_index);
                    m_requests[key] = data;
                }
            }
        }

    }

}

//
// Created by ZYXeeker on 2021/2/24.
//
#include <strings.h>
#include "http.h"

namespace http {

    // 从状态机
    HTTP_PACKET::HTTP_CODE HTTP_STATE_MACHINE::LINE_STATE(std::string str) {
//        switch (m_packet_state) {
//
//        }

        if (!str.compare(0, 7, "Accept:", 0, 7)) {
            std::cout << str << std::endl;
        }


    }

    // 请求方式状态机
    HTTP_PACKET::HTTP_CODE HTTP_STATE_MACHINE::METHOD_STATE(std::string str) {

    }

    // 报文参数状态机
    HTTP_PACKET::HTTP_CODE HTTP_STATE_MACHINE::HEADER_STATE(std::string str) {
        if (!str.compare(0, 7, "Accept:", 0, 7)) {
            std::cout << str << std::endl;
        }
        if (!str.compare(0, 7, "Referer:", 0, 8)) {
            std::cout << str << std::endl;
        }
        if (!str.compare(0, 7, "Accept-Language:", 0, 16)) {
            std::cout << str << std::endl;
        }
        if (!str.compare(0, 7, "Accept-Encoding:", 0, 16)) {
            std::cout << str << std::endl;
        }
        if (!str.compare(0, 7, "User-Agent:", 0, 11)) {
            std::cout << str << std::endl;
        }
        if (!str.compare(0, 7, "content-length:", 0, 15)) {
            std::cout << str << std::endl;
        }
        if (!str.compare(0, 7, "Host:", 0, 5)) {
            std::cout << str << std::endl;
        }
        if (!str.compare(0, 7, "Connection:", 0, 11)) {
            std::cout << str << std::endl;
        }
        if (!str.compare(0, 7, "Cookie:", 0, 7)) {
            std::cout << str << std::endl;
        }


    }

    // 报文内容状态机
    HTTP_PACKET::HTTP_CODE HTTP_STATE_MACHINE::CONTENT_STATE(std::string str) {

    }


    // 主状态机
    HTTP_PACKET::HTTP_CODE HTTP_STATE_MACHINE::MAIN_STATE(std::string str) {

        switch (m_packet_state) {
            case CHECK_METHOD: {
                HTTP_STATE_MACHINE::METHOD_STATE(str);
                break;
            }
            case CHECK_HEADER: {
                HTTP_STATE_MACHINE::HEADER_STATE(str);
                break;
            }
            case CHECK_CONTENT: {
                HTTP_STATE_MACHINE::CONTENT_STATE(str);
                break;
            }


        }
    }

    bool HTTP_STATE_MACHINE::put_packet_state(PACKET_STATE pState) {
        m_packet_state = pState;
    }


    HTTP_STATE_MACHINE::PACKET_STATE HTTP_STATE_MACHINE::m_packet_state = CHECK_METHOD;


    void Http::read_packet(char *packet) {
        int i;
        std::string str1 = packet;
//        std::cout << str1 << std::endl;
        int length = str1.length();
//        std::cout << length;

        // line
        for (i = 0; i < length; ++i) {
            if (str1[i] == '\r') {
                m_line = str1.substr(0, i);
                str1 = str1.substr(i + 2);
                std::cout << m_line << std::endl;
                HTTP_STATE_MACHINE::put_packet_state(HTTP_STATE_MACHINE::PACKET_STATE::CHECK_METHOD);
                HTTP_STATE_MACHINE::MAIN_STATE(m_line);
                break;
            }
        }

        // header
        int begin_index = 0;
        for (i = 0; i < str1.length();) {
            if (str1[i] == '\r') {
                m_header = str1.substr(begin_index, i - begin_index);
//                std::cout << m_header << std::endl;
                begin_index = i + 2;
                i = i + 2;
                HTTP_STATE_MACHINE::put_packet_state(HTTP_STATE_MACHINE::PACKET_STATE::CHECK_HEADER);
                HTTP_STATE_MACHINE::MAIN_STATE(m_header);
            } else
                i++;
        }


    }

}

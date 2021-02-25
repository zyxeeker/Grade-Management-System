//
// Created by ZYXeeker on 2021/2/22.
//

#ifndef WEBSEVER_HTTP_H
#define WEBSEVER_HTTP_H

#include <iostream>
#include <map>

namespace http {

    class HTTP_PACKET {
    public:
        enum HTTP_CODE {
            NO_REQUEST,
            GET_REQUEST,
            BAD_REQUEST,
            NO_RESOURCE,
            FORBIDDEN_REQUEST,
            FILE_REQUEST,
            INTERNAL_ERROR,
            CLOSED_CONNECTION
        };
        enum HTTP_METHOD {
            GET,
            POST,
            PUT,
            DELETE,
            HEAD,
            TRACE,
            OPTIONS,
            CONNECT,
            PATH
        };
        enum LINE_CODE {
            LINE_OK,
            LINE_ERROR,
            LINE_EMPTY
        };
    };

    class HTTP_STATE_MACHINE {
    public:
        // 主状态机(判断报文)
        enum PACKET_STATE {
            CHECK_METHOD,
            CHECK_HEADER,
            CHECK_CONTENT
        };
        // 从状态机(判断报文语句是否正确)
        enum LINE_STATE {
            LINE_OK,
            LINE_ERROR,
            LINE_EMPTY
        };

        static HTTP_PACKET::HTTP_CODE LINE_STATE(std::string str);

        HTTP_PACKET::HTTP_CODE PACKET_BLOCK_STATE();

        static HTTP_PACKET::HTTP_CODE METHOD_STATE(std::string str);

        static HTTP_PACKET::HTTP_CODE HEADER_STATE(std::string str);

        static HTTP_PACKET::HTTP_CODE CONTENT_STATE(std::string str);

        static PACKET_STATE m_packet_state;
    private:
    };

    class Http {
    public:
        void init();

        // 读取报文
        void read_packet(char *packet);

        void get_line();

        void get_header();

        void get_content();


        bool put_packet_state(HTTP_STATE_MACHINE::PACKET_STATE pState);

        HTTP_PACKET::HTTP_CODE MAIN_STATE(std::string str);

        // 解析报文请求
        HTTP_PACKET::LINE_CODE parse_method(std::string text);

        // 解析报文报头
        HTTP_PACKET::LINE_CODE parse_header(std::string text);

        // 解析报文内容
        HTTP_PACKET::LINE_CODE parse_content(std::string text);

    private:

        HTTP_STATE_MACHINE::PACKET_STATE m_packet_state;

        HTTP_PACKET::HTTP_METHOD m_method;
        std::string m_url;
        std::string m_protocol;
        std::string m_version;

        std::map<std::string, std::string> m_headers;

        std::map<std::string, std::string> m_requests;

        std::string m_body;

    };

}

#endif //WEBSEVER_HTTP_H

//
// Created by ZYXeeker on 2021/2/22.
//

#ifndef WEBSEVER_HTTP_H
#define WEBSEVER_HTTP_H

#include <iostream>

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

        static bool put_packet_state(PACKET_STATE pState);

        static HTTP_PACKET::HTTP_CODE MAIN_STATE(std::string str);

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

    private:
        // 解析报文请求行
        HTTP_PACKET::HTTP_CODE parse_line(char *line);

        // 解析报文报头
        HTTP_PACKET::HTTP_CODE parse_header(char *header);

        // 解析报文内容
        HTTP_PACKET::HTTP_CODE parse_content(char *content);

        std::string m_line;
        std::string m_header;
        std::string m_content;


    };

}

#endif //WEBSEVER_HTTP_H

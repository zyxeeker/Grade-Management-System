//
// Created by ZYXeeker on 2021/2/27.
//

#ifndef WEBSEVER_CONN_H
#define WEBSEVER_CONN_H

#include <sys/socket.h>
#include <sys/epoll.h>
#include "../http/http.h"
#include <arpa/inet.h>

namespace http_conn {
    class conn {
    public:
        void init(int MAX_CONNECTIONS, int port);

        int socket_init(int port);

        int epoll(int MAX_CONNECTIONS);

        bool socket_start(int MAX_CONNECTIONS);

    private:
        http::Http m_http_packet;

        struct epoll_event m_event;
        struct epoll_event *m_wait_event;

        int m_listen_fd;
        int m_epoll_fd;
        int m_conn;
        int m_recv_len;
        std::string m_header;

        char m_buff[4096];
        char m_clientIP[INET_ADDRSTRLEN] = "";
        struct sockaddr_in m_clientAddr;
        socklen_t m_clientAddrLen = sizeof(m_clientAddr);
    };
}

#endif //WEBSEVER_CONN_H

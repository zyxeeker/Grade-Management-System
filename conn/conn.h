//
// Created by ZYXeeker on 2021/2/27.
//

#ifndef WEBSEVER_CONN_H
#define WEBSEVER_CONN_H

#include <sys/socket.h>
#include <sys/epoll.h>
#include "../http/http.h"
#include <arpa/inet.h>
#include "../thread/thread.h"
#include <queue>

namespace http_conn {
    class conn {
    public:
        void init(int MAX_CONNECTIONS, int port);

        int socket_init(int port);

        int epoll();

        bool socket_start();

        void process();

    private:
        http::Http m_http_packet;
        threadPool::thread_pool<http_conn::conn> m_thread_pool;

        struct epoll_event m_event;
        struct epoll_event *m_wait_event;
        struct req {
            int num;
            std::string text;
        };

        int m_listen_fd;
        int m_epoll_fd;
        int m_conn;
        int m_recv_len;
        int m_MAX_CONNECTIONS;

        std::string m_header;

        char m_buff[4096];
        char m_clientIP[INET_ADDRSTRLEN] = "";
        struct sockaddr_in m_clientAddr;
        socklen_t m_clientAddrLen = sizeof(m_clientAddr);

        std::queue<req> client;
    };
}

#endif //WEBSEVER_CONN_H

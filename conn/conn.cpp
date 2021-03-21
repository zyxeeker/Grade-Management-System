//
// Created by ZYXeeker on 2021/2/27.
//

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "conn.h"

namespace http_conn {
    int conn::socket_init() {
        struct sockaddr_in addr;
        int on = 1;

        // socket
        if ((m_listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            std::cout << "ERROR" << std::endl;

        // bind
        addr.sin_family = AF_INET;
        addr.sin_port = htons(m_port);
        addr.sin_addr.s_addr = INADDR_ANY;

        //// socket参数
        // 允许端口复用
        setsockopt(m_listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

        if (bind(m_listen_fd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
            std::cout << "BIND　ERROR" << std::endl;

        // listen
        if (listen(m_listen_fd, 5) == -1) {
            std::cout << "LISTEN　ERROR" << std::endl;
        }
    }


    int conn::epoll() {
        m_epoll_fd = epoll_create1(EPOLL_CLOEXEC);

        m_event.data.fd = m_listen_fd;
        m_event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
        epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_listen_fd, &m_event);

        m_wait_event = new epoll_event[m_MAX_EVENTS];
    }

    void conn::send_packet(int client_num, std::string header) {
        send(client_num, header.c_str(), header.length(), 0);
        close(client_num);
    }

    bool conn::socket_start() {
        int i = 0;

        while (true) {
            int event_count = epoll_wait(m_epoll_fd, m_wait_event, m_MAX_EVENTS, -1);
            std::cout << "event:  " << event_count << std::endl;

            for (int j = 0; j < event_count; ++j) {
                std::cout << "j:  " << m_wait_event[j].data.fd << std::endl;

                if (m_wait_event[j].data.fd == m_listen_fd) {

                    while (true) {
                        m_conn = accept(m_listen_fd, (struct sockaddr *) &m_clientAddr, &m_clientAddrLen);
                        if (m_conn < 0) break;
                        if (m_conn > m_MAX_CONNECTIONS) break;
//                        std::cout << m_clientAddr.sin_addr.s_addr << std::endl;
//                        std::cout << m_clientAddr.sin_port << std::endl;
                        std::cout << std::endl;

                        memset(m_buff, 0, sizeof(m_buff));
                        m_recv_len = recv(m_conn, m_buff, sizeof(m_buff), 0);
                        m_buff[m_recv_len - 1] = '\0';

                        m_http_packet[m_conn].parse_packet(m_buff, m_conn);

                        m_thread_pool.append_work(m_http_packet + m_conn);
                    }
                }

            }
        }
        delete[] m_wait_event;
        close(m_listen_fd);
    }

    void conn::init(int MAX_EVENT_NUMBER, int MAX_CONNECTION_NUMBER, int port) {
        m_MAX_EVENTS = MAX_EVENT_NUMBER;
        m_MAX_CONNECTIONS = MAX_CONNECTION_NUMBER;
        m_port = port;
        m_http_packet = new http::Http[MAX_CONNECTION_NUMBER];

        socket_init();
        // epoll启动
        epoll();

        socket_start();

    }
}

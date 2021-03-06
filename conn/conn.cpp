//
// Created by ZYXeeker on 2021/2/27.
//

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "conn.h"

namespace http_conn {
    int conn::socket_init(int port) {
        struct sockaddr_in addr;
        int on = 1;

        // socket
        if ((m_listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
            std::cout << "ERROR" << std::endl;

        // bind
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
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
        m_event.events = EPOLLIN;
        epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_listen_fd, &m_event);
        m_wait_event = new epoll_event[m_MAX_CONNECTIONS];

    }

    void conn::process() {
        req re2;
        while (!client.empty()) {
            re2 = client.front();
            client.pop();
            // 解析报头
            m_http_packet.parse_packet(re2.text);
            // 返回
            m_header = m_http_packet.do_request();
            std::cout << re2.num << std::endl;
            send(re2.num, m_header.c_str(), m_header.length(), 0);
            close(re2.num);
        }
    }


    bool conn::socket_start() {
        int i = 0;
        while (true) {
//            m_thread_pool.append_work(this);
            int event_count = epoll_wait(m_epoll_fd, m_wait_event, m_MAX_CONNECTIONS, -1);

            for (int j = 0; j < event_count; ++j) {
                if (m_wait_event[j].data.fd == m_listen_fd) {
                    while (true) {
                        m_conn = accept(m_listen_fd, (struct sockaddr *) &m_clientAddr, &m_clientAddrLen);
                        if (m_conn < 0) break;

                        memset(m_buff, 0, sizeof(m_buff));
                        m_recv_len = recv(m_conn, m_buff, sizeof(m_buff), 0);
                        m_buff[m_recv_len - 1] = '\0';

                        req re1;
                        re1.num = m_conn;
                        re1.text = m_buff;

                        client.push(re1);
                        m_thread_pool.append_work(this);
                    }


                }
            }
            delete[] m_wait_event;
            close(m_listen_fd);
        }
    }

    void conn::init(int MAX_CONNECTIONS, int port) {
        m_MAX_CONNECTIONS = MAX_CONNECTIONS;

        socket_init(port);
        // epoll启动
        epoll();


        socket_start();

//        delete[] m_wait_event;
//        close(m_listen_fd);
    }
}

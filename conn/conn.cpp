//
// Created by ZYXeeker on 2021/2/27.
//

#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "conn.h"
#include "../http/http.h"

namespace http_conn {
    void conn::init(int port) {
        http::Http http_packet;

        http_packet.init();

        int listenfd, conn;
        struct sockaddr_in addr;
        char buff[4096];
        int len;
        int on = 1;

        // epoll
        struct epoll_event event;
        struct epoll_event *wait_event;
        int epoll_fd = epoll_create1(EPOLL_CLOEXEC);


        // socket
        if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            std::cout << "ERROR" << std::endl;
        }

        // bind
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        // 允许端口复用
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

        if (bind(listenfd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
            std::cout << "BIND　ERROR" << std::endl;

        // listen
        if (listen(listenfd, 5) == -1) {
            std::cout << "LISTEN　ERROR" << std::endl;
        }

        // epoll
        event.data.fd = listenfd;
        event.events = EPOLLIN;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listenfd, &event);



        // accept
        char clientIP[INET_ADDRSTRLEN] = "";
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);

        int i = 0;

        wait_event = new epoll_event[10];

        while (true) {
            int event_count = epoll_wait(epoll_fd, wait_event, 10, -1);

            for (int j = 0; j < event_count; ++j) {
                if (wait_event[j].data.fd == listenfd) {
                    event.events = EPOLLIN;
                    event.data.fd = conn;

                    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, epoll_fd, &event);
                    std::cout << "...listening" << std::endl;
                    conn = accept(listenfd, (struct sockaddr *) &clientAddr, &clientAddrLen);
                    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
                    std::cout << "...connect " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;

                    memset(buff, 0, sizeof(buff));
                    len = recv(conn, buff, sizeof(buff), 0);

                    buff[len - 1] = '\0';
                    if (strcmp(buff, "\0") == 0) {
                        std::cout << "...disconnect " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;
                        break;
                    }

//                    std::cout <<buff<<std::endl;

                    // 解析报头
                    http_packet.parse_packet(buff);
                    // 返回
                    std::string header = http_packet.do_request();
                    std::cout << header << std::endl;

                    send(conn, header.c_str(), header.length(), 0);
                }
                epoll_ctl(epoll_fd, EPOLL_CTL_DEL, conn, NULL);
                close(conn);
            }
        }
        delete[] wait_event;
        close(listenfd);
    }
}

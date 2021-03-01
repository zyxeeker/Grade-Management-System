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


        int listenfd, conn;
        struct sockaddr_in addr;
        char buff[4096];
        int len;
        int on = 1;

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

        // accept
        char clientIP[INET_ADDRSTRLEN] = "";
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int i = 0;
        while (true) {
            std::cout << "...listening" << std::endl;
            conn = accept(listenfd, (struct sockaddr *) &clientAddr, &clientAddrLen);
            if (conn < 0) {
                std::cout << "Error: accept" << std::endl;
                continue;
            } else
                inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
            std::cout << "...connect " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;


            while (true) {
                memset(buff, 0, sizeof(buff));
                len = recv(conn, buff, sizeof(buff), 0);

                buff[len - 1] = '\0';
                if (strcmp(buff, "\0") == 0) {
                    std::cout << "...disconnect " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;
                    break;
                }
                http_packet.parse_packet(buff);
            }
            close(conn);
        }
        close(listenfd);
    }
}

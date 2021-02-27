//
// Created by ZYXeeker on 2021/2/27.
//

#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "conn.h"

namespace conn {
    http::http(int port) {

        int listenfd, conn;
        struct sockaddr_in addr;
        char buff[4096];

        // socket
        if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            std::cout << "ERROR" << std::endl;
        }

        // bind
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

//        struct linger tmp = {0, 1};
//        setsockopt(listenfd, SOL_SOCKET, SO_LINGER, &tmp, sizeof(tmp));

        if (bind(listenfd, (struct sockaddr *) &addr, sizeof(addr)) == -1)
            std::cout << "ERROR" << std::endl;

        // listen
        if (listen(listenfd, 5) == -1) {
            std::cout << "ERROR" << std::endl;
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

            char buf[255];
            while (true) {
                memset(buf, 0, sizeof(buf));
                int len = recv(conn, buf, sizeof(buf), 0);
                buf[len] = '\0';
                if (strcmp(buf, "exit") == 0) {
                    std::cout << "...disconnect " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;
                    break;
                }
                std::cout << buf << std::endl;
                send(conn, buf, len, 0);
            }
            close(conn);
        }
        close(listenfd);
    }
}

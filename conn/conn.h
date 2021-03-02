//
// Created by ZYXeeker on 2021/2/27.
//

#ifndef WEBSEVER_CONN_H
#define WEBSEVER_CONN_H

#include <sys/socket.h>
#include <sys/epoll.h>

namespace http_conn {
    class conn {
    public:
        void init(int port = 9006);


    };
}

#endif //WEBSEVER_CONN_H

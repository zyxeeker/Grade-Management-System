//
// Created by ZYXeeker on 2021/2/24.
//

#ifndef WEBSEVER_THREAD_H
#define WEBSEVER_THREAD_H

#include <pthread.h>
#include <list>
#include "../locker/locker.h"

namespace threadPool {
    template<typename T>
    class thread_pool {
    public:
        thread_pool(int thread_num = 8, int max_requests = 100);

        ~thread_pool();

        bool init();

        bool append_work(T *req);

    private:
        static void worker(void *arg);

        void run();

    private:
        // 线程数
        int m_thread_num;
        // 最大请求数
        int m_max_requests;
        // 工作队列
        std::list<T *> m_work_queue;
        // 线程数组
        pthread_t *m_threads;
        // 锁
        locker m_locker;
        // 信号量
        sem m_sem;
        // 线程运行状态
        bool m_run_state;
    };
}
#endif //WEBSEVER_THREAD_H

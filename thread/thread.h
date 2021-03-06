//
// Created by ZYXeeker on 2021/2/24.
//

#ifndef WEBSEVER_THREAD_H
#define WEBSEVER_THREAD_H

#include <pthread.h>
#include <list>
#include "../locker/locker.h"
#include <iostream>

namespace threadPool {
    template<typename T>
    class thread_pool {
    public:
        thread_pool(int thread_num = 35, int max_requests = 100);

        ~thread_pool();


        bool append_work(T *req);

    private:
        static void *worker(void *arg);

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
        bool m_run_state = true;
    };

    template<typename T>
    thread_pool<T>::thread_pool(int thread_num, int max_requests) {
        m_thread_num = thread_num;
        m_max_requests = max_requests;

        // 创建存储线程类型的数组
        m_threads = new pthread_t[m_thread_num];

        // 创建线程
        for (int i = 0; i < m_thread_num; ++i) {
            if (pthread_create(m_threads + i, NULL, worker, this) != 0) {
                std::cout << "error" << std::endl;
                delete[] m_threads;
            }
            // 回收线程空间
            pthread_detach(m_threads[i]);

        }
    }

    template<typename T>
    thread_pool<T>::~thread_pool<T>() {
        delete[] m_threads;
    }

    // 推入工作队列
    template<typename T>
    bool thread_pool<T>::append_work(T *req) {
        m_locker.lock();

        m_work_queue.push_back(req);

        m_locker.unlock();

        m_sem.V();
        return true;
    }

    //
    template<typename T>
    void *thread_pool<T>::worker(void *arg) {
//        std::cout << "in thread"<<std::endl;
        thread_pool *pool = (thread_pool *) arg;
        pool->run();
        return pool;
    }

    // 线程运行
    template<typename T>
    void thread_pool<T>::run() {
        while (m_run_state) {

            m_sem.P();

            m_locker.lock();

            if (m_work_queue.empty()) {
                m_locker.unlock();
                continue;
            }

            T *req = m_work_queue.front();
            m_work_queue.pop_front();

            m_locker.unlock();

            req->process();
        }

    }

}
#endif //WEBSEVER_THREAD_H

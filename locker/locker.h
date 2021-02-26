//
// Created by ZYXeeker on 2021/2/26.
//

#ifndef WEBSEVER_LOCKER_H
#define WEBSEVER_LOCKER_H

#include <pthread.h>
#include <semaphore.h>
#include <exception>

// 信号量
class sem {
public:
    sem() {
        if (sem_init(&m_sem, 0, 0) != 0) throw std::exception();
    }

    sem(int num) {
        if (sem_init(&m_sem, 0, num)) throw std::exception();
    }

    ~sem() {
        sem_destroy(&m_sem);
    }

    // P信号
    bool P() {
        return sem_wait(&m_sem) == 0;
    }

    // V信号
    bool V() {
        return sem_post(&m_sem) == 0;
    }

private:
    sem_t m_sem;
};

// 互斥锁 mutex
class locker {
public:
    locker() {
        if (pthread_mutex_init(&m_mutex, NULL) != 0) throw std::exception();
    }

    ~locker() {
        pthread_mutex_destroy(&m_mutex);
    }

    // 加锁
    bool lock() {
        return pthread_mutex_lock(&m_mutex) == 0;
    }

    // 解锁
    bool unlock() {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }


private:
    pthread_mutex_t m_mutex;

};

#endif //WEBSEVER_LOCKER_H

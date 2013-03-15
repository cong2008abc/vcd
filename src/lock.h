#ifndef _LOCK_H_
#define _LOCK_H_

#include "define.h"
#include <pthread.h>
#include <stdio.h>

namespace vcd {

//typedef thread_mutex_t MutexLock;
class MutexLock {
public:
    MutexLock() {
        pthread_mutex_init(&linux_mutex, NULL);
    }

    ~MutexLock() {
    }

    void Lock() {
        pthread_mutex_lock(&linux_mutex);
    }

    void UnLock() {
        pthread_mutex_unlock(&linux_mutex);
    }

private:
    pthread_mutex_t linux_mutex;
};

class MutexLockGuard {
public:
    explicit MutexLockGuard(MutexLock &mutex)
    : locker_(mutex) {
        locker_.Lock();
        //printf("Lock!\n");
    }

    ~MutexLockGuard() {
        locker_.UnLock();
//        fprintf(stderr, "UnLock!\n");
    }
private:
    MutexLock &locker_;
    DISALLOW_COPY_AND_ASSIAGN(MutexLockGuard);
};

} // namespace vcd
#endif

#ifndef _MUTEX_LOCK_H_
#define _MUTEX_LOCK_H_

#include "mutex.h"
#include <memory>

class Mutex_lock
{
public:
    Mutex_lock(Mutex *pm) : mutex_ptr_(pm)
    {
        mutex_ptr_->lock();
    }

    ~Mutex_lock()
    {
        mutex_ptr_->unlock();
    }

private:
    Mutex *mutex_ptr_;
};

#endif

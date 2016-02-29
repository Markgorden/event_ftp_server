#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "uncopyable.h"

class Condition;

class Mutex : private Uncopyable
{
public:
    Mutex();
    ~Mutex();
    void lock();
    void unlock();

private:
    friend class Condition;
    pthread_mutex_t mu_;
};

inline void pthread_call(const char *label, int result)
{
    if(result != 0)
    {
        fprintf(stderr, "thread %s: %s", label, strerror(result));
        abort();
    }
}

#endif

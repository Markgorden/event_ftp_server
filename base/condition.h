#ifndef _CONDITION_H
#define _CONDITION_H

#include <pthread.h>
#include "mutex.h"
#include "uncopyable.h"

class Mutex;

class Condition : private Uncopyable
{
public:
    Condition();
    virtual ~Condition();
    void notify();
    void wait(Mutex *mu);
    void broadcast();

private:
    pthread_cond_t cond;
};

#endif

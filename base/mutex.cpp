#include "mutex.h"

Mutex::Mutex()
{
    pthread_call("mutex init", pthread_mutex_init(&mu_, NULL));
}


Mutex::~Mutex()
{
    pthread_call("mutex destroy", pthread_mutex_destroy(&mu_));
}

void Mutex::lock()
{
    pthread_call("mutex lock", pthread_mutex_lock(&mu_));
}

void Mutex::unlock()
{
    pthread_call("mutex unlock", pthread_mutex_unlock(&mu_));
}



#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "thread.h"
#include "mutex.h"
#include "condition.h"
#include <queue>

enum
{
    ERR_THREAD_POOL = 0x100,
    ERR_TASK_QUEUE_FULL,
};

typedef int (*task_handler_t)(void *arg);

struct task_body
{
    task_handler_t task_handler;
    void *arg;
};


class Pool_work_thread : public Thread
{
public:
    void run();
};


class Thread_pool
{
public:	
    static const int thread_quantity = 4;
    static const int task_queue_size = 128;

    Thread_pool();
    ~Thread_pool();
    void start();
    int add_task(task_handler_t , void *);
    bool is_task_full() const;
    bool is_task_empty() const;
    void run_in_thread();
	
	
private:
    Thread_pool(const Thread_pool &);
    Thread_pool &operator=(const Thread_pool &);
	
    Pool_work_thread work_threads[thread_quantity];
    std::queue<task_body *> task_queue;
	
    int queue_len;
    bool shutdown;
    Mutex mutex;
    Condition cond;
};

#endif

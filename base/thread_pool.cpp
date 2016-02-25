#include "error.h"
#include "thread_pool.h"
#include "mutex_lock.h"
#include <iostream>


using namespace std;


void Pool_work_thread::run()
{
	Thread_pool *thr_pool = (Thread_pool *)arg;
	
	thr_pool->run_in_thread();
	
    exit();
}


Thread_pool::Thread_pool()
{
	queue_len = 0;
	shutdown  = true;
}

Thread_pool::~Thread_pool()
{
	int i;

	{
		Mutex_lock lock(&mutex);

		shutdown = true;
		cond.broadcast();
	}


	for(i=0;i<thread_quantity;i++)
	{
		work_threads[i].join();
	}

	if(!is_task_empty())
	{
		while(queue_len > 0)
		{
			struct task_body *tb;

			tb = task_queue.front();
			task_queue.pop();
			delete tb;
		}
	}
}

void Thread_pool::start()
{
	int i;

	shutdown = false;

	for(i=0;i<thread_quantity;i++)
	{
		work_threads[i].start(this);
	}
}

bool Thread_pool::is_task_full() const
{
	return queue_len == task_queue_size;
}

bool Thread_pool::is_task_empty() const
{
	return queue_len == 0;
}

int Thread_pool::add_task(task_handler_t handler, void *arg)
{
	struct task_body *task;

	Mutex_lock lock(&mutex);
	
	if(is_task_full())
	{
		return ERR_TASK_QUEUE_FULL;
	}

	task = new struct task_body();
	if(task == NULL)
	{
		return ERR_NOMEM;
	}

	task->task_handler = handler;
	task->arg = arg;

	task_queue.push(task);
	queue_len++;

	cond.notify();

	return 0;
}


void Thread_pool::run_in_thread()
{
	while(shutdown == false)
	{
		struct task_body *task_body;
		
		{
			Mutex_lock lock(&mutex);
		
			while(is_task_empty() && shutdown == false)
			{
				cond.wait(&mutex);
			}

			//waked up to be destroyed
			if(shutdown == true)
			{	
				break;
			}

			task_body = task_queue.front();
			task_queue.pop();
			queue_len--;	
		}
		
		task_body->task_handler(task_body->arg);
		
		delete task_body;			
	}
}



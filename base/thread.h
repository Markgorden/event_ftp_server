#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>
#include "base/uncopyable.h"

class Thread : public Uncopyable
{

public:
	Thread();
	void start(void *arg);
	void exit();
    void join();
	virtual ~Thread();
	virtual void run() = 0;
	
protected:
	void *arg;

private:
	static void *run_thread(void *);
	
	int started;
	pthread_t tid;
};


#endif

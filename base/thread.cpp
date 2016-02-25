#include <iostream>
#include "thread.h"

using namespace std;

Thread::Thread()
{
    cout<<"Thread::Thread()"<<endl;
    started = 0;
}

Thread::~Thread()
{
    cout<<"Thread::~Thread()"<<endl;
}


void Thread::start(void *arg)
{
    if(started == 1)
    {
        cout<<"Thread already started"<<endl;
 		return;
    }

    this->arg = arg;

    if(pthread_create(&tid, NULL, run_thread, this) != 0)
    {
        cout<<"Thread create error"<<endl;
    	throw "error";
    }
}

void Thread::exit()
{
	pthread_exit(NULL);
}


void Thread::join()
{
    pthread_join(tid, NULL);
}


void *Thread::run_thread(void *arg)
{
     ((Thread *)arg)->run();

	 return NULL;
}






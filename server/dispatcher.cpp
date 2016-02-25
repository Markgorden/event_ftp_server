#include "server/dispatcher.h"
#include <iostream>
#include "base/file_opr.h"


Dispatcher::Dispatcher(Event_server *ser)
{
    last_recv_thread_ = -1;

	for(int i=0; i<WORKER_THREAD_NUM; i++)
	{
		worker_threads_[i].set_event_server(ser);
	}
}

Dispatcher::~Dispatcher()
{
	
}

void Dispatcher::dispatch(Conn *c)
{
	char buf[1];
	
    int chosen = (last_recv_thread_ + 1) % WORKER_THREAD_NUM;

    last_recv_thread_ = chosen;

	std::cout << "dispatch connection to thread" << chosen << std::endl;

    worker_threads_[chosen].add_conn(c);

	buf[0] = 'c';
	file::writen(worker_threads_[chosen].get_notify_send_fd(), buf, 1);
}

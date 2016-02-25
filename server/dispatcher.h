#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_

#include "server/libevent_thread.h"
#include "base/uncopyable.h"

#define WORKER_THREAD_NUM 8

class Libevent_thread;
class Conn;
class Event_server;

class Dispatcher : public Uncopyable
{
public:
	Dispatcher(Event_server *ser);
    ~Dispatcher();
	void dispatch(Conn *connection);
	
private:
	Libevent_thread worker_threads_[WORKER_THREAD_NUM];
	int last_recv_thread_;
	
};

#endif



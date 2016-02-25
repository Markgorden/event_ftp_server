#include "server/libevent_thread.h"
#include <unistd.h>
#include <assert.h>
#include <iostream>
#include <event2/bufferevent.h>
#include "server/conn.h"
#include "server/event_server.h"



Libevent_thread::Libevent_thread()
{
	int pipefd[2];
	
	assert((base_ = event_base_new()) != NULL);

	//create pipe to receive dispatched connection from main thread
	assert((pipe(pipefd) == 0));
	notify_send_fd_ = pipefd[1];
	notify_receive_fd_ =  pipefd[0];
	
	assert(event_assign(&notify_event_, base_, notify_receive_fd_, 	EV_READ | EV_PERSIST, 
		handle_notify, static_cast<void *>(this)) == 0);
	assert(event_add(&notify_event_, NULL) == 0);

    start(NULL);
}


Libevent_thread::~Libevent_thread()
{
	
}


void Libevent_thread::run(void)
{
	std::cout << "thread event base run" << std::endl;
	event_base_loop(base_, 0);
}



Event_server *Libevent_thread::get_event_server()
{
    return server_;
}

void Libevent_thread::set_event_server(Event_server * ser)
{
	server_ = ser;
}

int Libevent_thread::get_notify_send_fd()
{
	return notify_send_fd_;
}


struct event_base *Libevent_thread::get_event_base()
{
	return base_;
}


void Libevent_thread::add_conn(Conn *connection)
{
	cq_.push(connection);
	connection->set_attached_worker(this);
}


void Libevent_thread::accept_new_connection(void)
{
	Conn *c = cq_.front();
	struct bufferevent *bev;
	
	cq_.pop();

	if(c == NULL)
	{
		std::cout << "connection queue is empty!" << std::endl;
		return;
	}

	bev = bufferevent_socket_new(base_, c->get_socket_fd(), BEV_OPT_CLOSE_ON_FREE);
	if(bev == NULL)
	{
		fprintf(stderr, "alloc bufferevent error");
		return;
	}

	c->set_bev(bev);

	bufferevent_setcb(bev, server_->read_event_cb, server_->write_event_cb,
                server_->error_event_cb, static_cast<void *>(c));
	bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);
}


void Libevent_thread::handle_notify(int fd, short event, void *arg)
{
	Libevent_thread *thread = static_cast<Libevent_thread *>(arg);
	char buf[1];

	std::cout << "received notify" << std::endl;

	if(read(fd, &buf, 1) < 0)
	{
		fprintf(stderr, "cannot read from libevent pipe");
	}

	switch(buf[0])
	{
		case 'c': //new connection arrives
			thread->accept_new_connection();
			break;
		
	}

	
}


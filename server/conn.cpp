#include "server/conn.h"

Conn::Conn()
{
	
}


Conn::~Conn()
{
	bufferevent_free(bev_);
}

Libevent_thread *Conn::get_attached_worker()
{
	return attached_worker_;
}

int Conn::get_socket_fd()
{
	return fd_;
}

void Conn::set_attached_worker(Libevent_thread * worker)
{
	attached_worker_ = worker;
}

struct bufferevent *Conn::get_bev()
{
	return bev_;
}

void Conn::set_bev(struct bufferevent *bev)
{
	bev_ = bev;
}
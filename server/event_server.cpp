#include "server/event_server.h"
#include "network/sock_opr.h"
#include "network/net_addr.h"
#include "server/conn.h"
#include "server/dispatcher.h"
#include <iostream>
#include <assert.h>



Event_server::Event_server(int port)
{
    Net_addr local_addr(port);

    listener_.set_reuse_addr(true);
    listener_.bind_addr(local_addr);
    socks::set_nonblock(listener_.get_sockfd());
    listener_.listen();
}



Event_server::~Event_server()
{
	
}



void Event_server::run()
{
    assert((dispatcher_ = new Dispatcher(this)) != NULL);

    assert((main_base_ = event_base_new()) != NULL);
    assert(event_assign(&listen_event_, main_base_, listener_.get_sockfd(), EV_READ | EV_PERSIST,
           Event_server::accept_event_cb, this) == 0);
    assert(event_add(&listen_event_, NULL) == 0);

    event_base_dispatch(main_base_);
}



void Event_server::accept_event_cb(evutil_socket_t listener, short event, void *arg)
{
    int conn_fd;
    struct sockaddr_in addr;	
    char buf[32];
    Event_server *ser = static_cast<Event_server*>(arg);
    Conn *connection;
	
    conn_fd = socks::accept(listener, &addr);
    if(conn_fd < 0)
    {
        return;
    }

    socks::to_ip_port(buf, sizeof(buf), addr);
    std::cout << "connect from " << buf <<std::endl;
		
    connection = ser->handle_accept_event(conn_fd);

    connection->fd_ = conn_fd;

    printf("socket-->%d, c-->%p\n",connection->get_socket_fd(), connection);

    ser->dispatcher_->dispatch(connection);
}



void Event_server::read_event_cb(struct bufferevent *bev, void *arg)
{
    Conn *connection = static_cast<Conn *>(arg);
    Event_server *ser = connection->get_attached_worker()->get_event_server();

    std::cout << "read event" << std::endl;

    ser->handle_read_event(connection);
}



void Event_server::write_event_cb(struct bufferevent *bev, void *arg)
{
    Conn *connection = static_cast<Conn *>(arg);
    Event_server *ser = connection->get_attached_worker()->get_event_server();

    std::cout << "write event" << std::endl;

    ser->handle_write_event(connection);
}



void Event_server::error_event_cb(struct bufferevent *bev, short event, void *arg)
{
    Conn *connection = static_cast<Conn *>(arg);
    Event_server *ser = connection->get_attached_worker()->get_event_server();

    ser->handle_error_event(connection);
}


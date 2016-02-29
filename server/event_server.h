#ifndef _EVENT_SERVER_H_
#define _EVENT_SERVER_H_

#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/event_struct.h>
#include "base/uncopyable.h"
#include "network/socket.h"


class Conn;
class Dispatcher;


class Event_server : public Uncopyable
{
public:	
    Event_server(int port);
    virtual ~Event_server();

    void run();
	
    static void accept_event_cb(evutil_socket_t fd, short event, void *arg);
    static void read_event_cb(struct bufferevent *bev, void *arg);
    static void write_event_cb(struct bufferevent *bev, void *arg);
    static void error_event_cb(struct bufferevent *bev, short event, void *arg);
	
    virtual Conn *handle_accept_event(int fd) = 0;
    virtual void handle_read_event(Conn *connection) = 0;
    virtual void handle_write_event(Conn *connection) = 0;
    virtual void handle_error_event(Conn *connection) = 0;
	
private:
    Dispatcher *dispatcher_;//dispatch connection to libevent threads
    struct event_base *main_base_;
    struct event listen_event_;//listen event for new tcp connection
    Socket listener_;
};

#endif

#ifndef _CONN_QUEUE_H_
#define _CONN_QUEUE_H_

#include <event2/event_struct.h>
#include <event2/bufferevent.h>
#include "base/uncopyable.h"


class Libevent_thread;

class Conn : public Uncopyable
{
    friend class Event_server;
	
public:
    Conn();
    virtual ~Conn();
    Libevent_thread *get_attached_worker();
    int get_socket_fd(); 
    struct bufferevent *get_bev();
    void set_bev(struct bufferevent *bev);
    void set_attached_worker(Libevent_thread *worker);
	
private:
    int fd_; //socket fd for this connection
    Libevent_thread *attached_worker_;
    struct bufferevent *bev_;
};


#endif

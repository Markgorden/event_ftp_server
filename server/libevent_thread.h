#ifndef _LIBEVENT_THREAD_H_
#define _LIBEVENT_THREAD_H_

#include <event2/event.h>
#include <event2/event_struct.h>
#include <queue>
#include "base/thread.h"


class Event_server;
class Conn;

class Libevent_thread : public Thread
{
    static void handle_notify(int fd, short event, void *arg);
public:
    Libevent_thread();
    ~Libevent_thread();
    void run(void);
    void add_conn(Conn *connection);
    Event_server *get_event_server();
    void set_event_server(Event_server *ser);
    int get_notify_send_fd();
    struct event_base *get_event_base();
	
private:
    Event_server *server_; 
    struct event_base *base_;//libevent handle for this thread
    std::queue<Conn *> cq_;			 //to store connection from the dispatch thread
    int notify_receive_fd_;  //receiving end for notify pipe
    int notify_send_fd_;     //sending end for notify pipe 
    struct event notify_event_;//listen event for notify pipe
    void accept_new_connection();
};

#endif

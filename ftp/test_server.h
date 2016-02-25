#ifndef _TEST_SERVER_H_
#define _TEST_SERVER_H_

#include "server/event_server.h"

class Conn;

class Test_server : public Event_server
{
public:
    Test_server(int port);
    ~Test_server();
    Conn *handle_accept_event(int fd);
    void handle_read_event(Conn *connection);
    void handle_write_event(Conn *connection);
    void handle_error_event(Conn *connection);
};

class Test_conn : public Conn
{
public:
	char buf[256];
};

#endif

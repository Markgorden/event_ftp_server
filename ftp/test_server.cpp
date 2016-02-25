/*************a simple echo server*************/


#include "server/event_server.h"
#include "server/conn.h"
#include "network/sock_opr.h"
#include "ftp/test_server.h"
#include <iostream>
#include <stdio.h>
#include <event2/bufferevent.h>
#include <string.h>


Test_server::Test_server(int port) : Event_server(port)
{
    std::cout<<"welecome to the test echo server"<<std::endl;
}

Test_server::~Test_server()
{

}

Conn *Test_server::handle_accept_event(int fd)
{
	Conn *connection = new Test_conn;

	return connection;
}


void Test_server::handle_read_event(Conn *connection)
{
	size_t n;
	Test_conn *c = static_cast<Test_conn *>(connection);
	
	n = bufferevent_read(c->get_bev(), c->buf, sizeof(c->buf));
	if(n <= 0)
	{
		fprintf(stderr, "error on bufferevent_read");
		return ;
	}
	c->buf[n] = 0;

	printf("received:%s", c->buf);	

	bufferevent_write(c->get_bev(), c->buf, strlen(c->buf));
}


void Test_server::handle_write_event(Conn *connection)
{
	
}


void Test_server::handle_error_event(Conn *connection)
{
	std::cout << "peer closed connection" << std::endl;
}

#if 0
int main()
{
	Test_server test_server(8888);

	test_server.run();

	return 0;
}
#endif

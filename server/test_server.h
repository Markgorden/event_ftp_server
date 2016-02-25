#ifndef _TEST_SERVER_H_
#define _TEST_SERVER_H_

#include "server/event_server.h"

class Test_server : public Event_server
{
public:
	Test_server(int port);
	~Test_server();
	void run();

private:
	
};

#endif

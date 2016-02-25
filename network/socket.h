#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <stdint.h>
#include <string>

class Net_addr;

class Socket
{
public:
	Socket();
	Socket(int skfd):sockfd(skfd)
	{ }

	~Socket();
	
	void listen() const;
	int  accept(Net_addr &addr) const;
	void bind_addr(Net_addr &addr) const;
	void set_reuse_addr(bool enable) const;
	void set_keep_alive(bool enable) const;
	int get_sockfd();
	int get_socket_port(unsigned short *port);
	int get_socket_ip(uint32_t *ip);
	int get_socket_ip(std::string &ip);
	
private:
	int sockfd;
};

#endif



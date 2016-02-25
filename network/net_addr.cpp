#include "net_addr.h"
#include "sock_opr.h"
#include <string.h>

Net_addr::Net_addr(uint16_t port)
{
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = socks::host_to_network32(INADDR_ANY);
	addr.sin_port = socks::host_to_network16(port);
}

Net_addr::Net_addr(std::string ip, uint16_t port)
{
	memset(&addr, 0, sizeof(addr));

	socks::from_ip_port(ip.c_str(), port, addr);
}

struct sockaddr_in &Net_addr::get_net_addr()
{
    return addr;
}


std::string Net_addr::get_ip_port() const
{
	char buf[32];

	socks::to_ip_port(buf, sizeof(buf), addr);

	return buf;
}

std::string Net_addr::get_ip() const
{
	char buf[32];

	socks::to_ip(buf, sizeof(buf), addr);

	return buf;
}


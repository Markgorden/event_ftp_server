#ifndef _NET_ADDR_H_
#define _NET_ADDR_H_

#include "sys/socket.h"
#include "netinet/in.h"
#include <stdint.h>
#include <string>


class Net_addr
{
public:
    Net_addr()
    {}
    Net_addr(uint16_t port);
    Net_addr(std::string ip, uint16_t port);
    void set_net_addr(const struct sockaddr_in &sa);
    struct sockaddr_in &get_net_addr();
    std::string get_ip_port() const;
    std::string get_ip() const;

private:
    struct sockaddr_in addr;
};

#endif

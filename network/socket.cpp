#include "socket.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "base/base.h"
#include "network/sock_opr.h"
#include "network/net_addr.h"
#include <assert.h>


Socket::Socket()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockfd > 0);
    socks::set_nonblock(sockfd);
}

Socket::~Socket()
{
    close(sockfd);
}

void Socket::listen() const
{
    socks::listen(sockfd);
}

void Socket::bind_addr(Net_addr &addr) const
{
    socks::bind(sockfd, addr.get_net_addr());
}

int Socket::accept(Net_addr &addr) const
{
   return socks::accept(sockfd, &addr.get_net_addr());
}

void Socket::set_reuse_addr(bool enable) const
{
    int optval = enable ? 1 : 0;

    ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
            &optval, static_cast<socklen_t>(sizeof(optval)));
}


void Socket::set_keep_alive(bool enable) const
{
    int optval = enable ? 1 : 0;

    ::setsockopt(sockfd, IPPROTO_TCP, SO_KEEPALIVE,
            &optval, static_cast<socklen_t>(sizeof(optval)));
}


int Socket::get_sockfd()
{
    return sockfd;
}

int Socket::get_socket_port(unsigned short * port)
{
    return socks::get_port(sockfd, port);
}

int Socket::get_socket_ip(uint32_t *ip)
{
    return socks::get_ip_addr(sockfd, ip);
}

int Socket::get_socket_ip(std::string &s)
{
    char buf[32];

    socks::get_ip_addr(sockfd, buf, sizeof(buf));

    s.assign(buf);
    return 0;
}
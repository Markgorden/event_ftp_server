#include "sock_opr.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>


struct sockaddr *socks::sockaddr_cast(struct sockaddr_in *addr)
{
    return reinterpret_cast<struct sockaddr *>(addr);
}

const struct sockaddr *socks::sockaddr_cast(const struct sockaddr_in *addr)
{
    return reinterpret_cast<const struct sockaddr *>(addr);
}

int socks::create_tcp_socket()
{
    int sockfd;

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockfd < 0)
    {
        fprintf(stderr, "socks::create_tcp_socket error\n");
    }

    return sockfd;
}


void socks::set_nonblock(int fd)
{
    int flags;

    flags = fcntl(fd, F_GETFL, 0);
	
    flags |= O_NONBLOCK;
	
    fcntl(fd, F_SETFL, flags);
}


int socks::bind(int sockfd, const struct sockaddr_in &addr)
{
    int ret;

    ret = ::bind(sockfd, sockaddr_cast(&addr), static_cast<socklen_t>(sizeof(addr)));
    if(ret < 0)
    {
        fprintf(stderr, "bind error");
    }
  
    return ret;
}


int socks::listen(int sockfd)
{
    int ret;

    ret = ::listen(sockfd, SOMAXCONN);
    if(ret < 0)
    {
        fprintf(stderr, "socks::listen error");
    }

    return ret;
}


int socks::accept(int sockfd, struct sockaddr_in *addr)
{
    socklen_t addrlen = static_cast<socklen_t>(sizeof(*addr));
    int conn_fd;

    conn_fd = ::accept(sockfd, sockaddr_cast(addr), &addrlen);
    if(conn_fd < 0)
    {
        fprintf(stderr, "accpet error");
        return conn_fd;
    }

    set_nonblock(conn_fd);
	
    return conn_fd;
}


int socks::connect(int sockfd, const struct sockaddr_in &addr)
{
    int conn_fd;

    conn_fd = ::connect(sockfd, sockaddr_cast(&addr), static_cast<socklen_t>(sizeof(addr)));
    if(conn_fd < 0)
    {
        fprintf(stderr, "connect error");
    }

    return conn_fd;
}


int socks::close(int sockfd)
{
    int ret;

    if((ret = ::close(sockfd)) < 0)
    {
        fprintf(stderr, "close error");
    }

    return ret;
}


ssize_t socks::read(int sockfd, void *buf, size_t count)
{
    return ::read(sockfd, buf, count);
}

ssize_t socks::write(int sockfd, const void *buf, size_t count)
{
    return ::write(sockfd, buf, count);
}


uint16_t socks::network_to_host16(uint16_t n)
{
    return ntohs(n);
}

uint32_t socks::network_to_host32(uint32_t n)
{
    return ntohl(n);
}

uint16_t socks::host_to_network16(uint16_t n)
{
	return htons(n);
}

uint32_t socks::host_to_network32(uint32_t n)
{
    return htons(n);
}

void socks::to_ip_port(char *buf, size_t size, const struct sockaddr_in &addr)
{
    size_t end;
    uint16_t port;
	
    assert(size > INET_ADDRSTRLEN);
	
    inet_ntop(AF_INET, &addr.sin_addr, buf, static_cast<socklen_t>(size));
    port = network_to_host16(addr.sin_port);

    end = strlen(buf);

    snprintf(buf+end, size-end, ":%u", port);
}

void socks::to_ip(char *buf, size_t size, const struct sockaddr_in &addr)
{
    assert(size > INET_ADDRSTRLEN);
	
    inet_ntop(AF_INET, &addr.sin_addr, buf, static_cast<socklen_t>(size));
}

void socks::from_ip_port(const char *ip, uint16_t port, struct sockaddr_in &addr)
{
    addr.sin_family = AF_INET;
    addr.sin_port   = host_to_network16(port);
    if(inet_pton(AF_INET, ip, &addr.sin_addr) < 0)
    {
        fprintf(stderr, "inet_pton error");
    }
}


int socks::get_port(int socket,unsigned short * port)
{
    struct sockaddr_in addr;
    int ret;
    int len = sizeof(addr);
    
    ret = getsockname(socket, (struct sockaddr *)&addr, reinterpret_cast<socklen_t *>(&len));
    if(ret != 0)
    {
        return ret;
    }

    *port = network_to_host16(addr.sin_port);

    return 0;
}


int socks::get_ip_addr(int socket, uint32_t *ip)
{
    struct sockaddr_in addr;
    int ret;
    int len = sizeof(addr);

    ret = getsockname(socket, (struct sockaddr *)&addr, reinterpret_cast<socklen_t *>(&len));
    if(ret != 0)
    {
        return ret;
    }

    *ip = network_to_host32(static_cast<uint32_t>(addr.sin_addr.s_addr));

    return 0;
}


int socks::get_ip_addr(int socket, char *buf, size_t size)
{   
    struct sockaddr_in addr;
    int ret;
    int len = sizeof(addr);

    ret = getsockname(socket, (struct sockaddr *)&addr, reinterpret_cast<socklen_t *>(&len));
    if(ret != 0)
    {
        return ret;
    }

    assert(size > INET_ADDRSTRLEN);
	
    inet_ntop(AF_INET, &addr.sin_addr, buf, static_cast<socklen_t>(size));

    return 0;
}

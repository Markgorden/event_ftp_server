#ifndef _SOCK_OPR_H_
#define _SOCK_OPR_H_

#include <sys/socket.h> 
#include <stdint.h>
#include "netinet/in.h"


namespace socks
{
	
struct sockaddr *sockaddr_cast(struct sockaddr_in *addr);
const struct sockaddr *sockaddr_cast(const struct sockaddr_in *addr);
int create_tcp_socket();
void set_nonblock(int fd);
int bind(int sockfd, const struct sockaddr_in &addr);
int listen(int sockfd);
int accept(int sockfd, struct sockaddr_in *addr);
int connect(int sockfd, const struct sockaddr_in &addr);
int close(int sockfd);
ssize_t read(int sockfd, void *buf, size_t count);
ssize_t write(int sockfd, const void *buf, size_t count);
uint16_t network_to_host16(uint16_t n);
uint32_t network_to_host32(uint32_t n);
uint16_t host_to_network16(uint16_t n);
uint32_t host_to_network32(uint32_t n);
void to_ip_port(char *buf, size_t size, const struct sockaddr_in &addr);
void to_ip(char *buf, size_t size, const struct sockaddr_in &addr);
void from_ip_port(const char *ip, uint16_t port, struct sockaddr_in &addr);
int get_port(int socket, unsigned short *port);
int get_ip_addr(int socket, uint32_t *ip);
int get_ip_addr(int socket, char *buf, size_t size);

}

#endif

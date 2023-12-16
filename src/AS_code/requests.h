#ifndef GETID_H
#define GETID_H

#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <wait.h>
#include "../validations.h"

#define DEFAULT_PORT "58023"
#define MAX_BUFFER_SIZE 128
#define CMD_SIZE 3
#define MAX_STATUS_SIZE 3
#define SIZE_PATH_USER_DIR 25
#define MAX_NUM_AID 999
#define TIMEOUT 1

void handle_requests(char *port);
int do_socket(int socket_type);
void do_bind(int socket_type, struct sockaddr_in *addr);
void initialize_addr(struct sockaddr_in *addr, char *port);
void handle_udp_socket(int udp_socket, struct sockaddr_in udp_addr);
void handle_tcp_socket(int tcp_socket, struct sockaddr_in tcp_addr);
void execute_request_udp(int fd, struct sockaddr_in addr, char* msg);
void execute_request_tcp(int fd, struct sockaddr_in addr, char* msg);
int read_request_tcp(char *src, int fd, int spaces);
void handle_SIGINT(int SIGNAL);
void handle_SIGCHLD(int SIGNAL);
void set_timeout(int fd, int optname);

#endif
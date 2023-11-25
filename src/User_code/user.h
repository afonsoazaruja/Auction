#ifndef USER_H
#define USER_H

#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "getIp.h"
#include "commands.h"
#include <fcntl.h>


#define DEFAULT_PORT "58023" // 58000 + 23
#define BUFFER_SIZE 10000


int main(int argc, char **argv);
void send_request_udp(char *port, char *asip, char *buffer);
void analyze_reply_udp(int fd, char *buffer);
void send_request_tcp(char *port, char *asip, char *buffer);

#endif

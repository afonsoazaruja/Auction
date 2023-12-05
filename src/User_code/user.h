#ifndef USER_H
#define USER_H

#include "commands.h"
#include "validations.h"
#include <netdb.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define DEFAULT_PORT "58023" // 58000 + 23
#define BUFFER_SIZE 7000

int main(int argc, char **argv);
void send_request_udp(char *port, char *asip, char *buffer);
void send_request_tcp(char *port, char *asip, char *buffer);
char* getIpAddress();

#endif

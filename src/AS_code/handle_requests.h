#ifndef GETID_H
#define GETID_H

#include <stdlib.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../validations.h"


#define CMD_SIZE 3
#define MAX_STATUS_SIZE 3
#define SIZE_PATH_USER_DIR 25

void handle_requests_udp(char* port, bool verbose);
void handle_requests_tcp(char* port, bool verbose);
void execute_request_udp(int fd, struct sockaddr_in addr, char* msg);

#endif
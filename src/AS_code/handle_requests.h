#ifndef GETID_H
#define GETID_H

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#ifndef __cplusplus // To avoid conflict with C++ bool type
typedef enum {
    false = 0,
    true = 1
} bool;
#endif

void handle_requests_udp(char* port, bool verbose);
void handle_requests_tcp(char* port, bool verbose);

#endif
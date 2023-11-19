#include <stdio.h>
#include <errno.h>
#include "handle_requests.h"
#define DEFAULT_PORT "58023"


int main(int argc, char **argv) {
    bool verbose = false;
    ssize_t n;
    socklen_t addrlen;
    int fd, errcode;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];
    char port[6] = DEFAULT_PORT;
    
    // Update port and/or set verbose mode
    if (argc > 1) {
        if (!strcmp(argv[1], "-p")) {
            memcpy(port, argv[2], strlen(argv[2]) + 1);
            if (argc == 4 && !strcmp(argv[3], "-v")) verbose = true;
        } else if (!strcmp(argv[1], "-v")) verbose = true;  
    }
    handle_requests_udp(port, verbose);
}

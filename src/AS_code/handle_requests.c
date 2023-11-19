#include "handle_requests.h"

void handle_requests_udp(char *port, bool verbose) {
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];

    fd = socket(AF_INET, SOCK_DGRAM, 0); 
    if (fd == -1) /*error*/ exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       
    hints.ai_socktype = SOCK_DGRAM;  
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(NULL, port, &hints, &res);
    if (errcode != 0) /*error*/ exit(1);

    n = bind(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) /*error*/ exit(1);

    while (true) {
        addrlen = sizeof(addr);
        n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*)&addr, &addrlen);
        if (n == -1) /*error*/ exit(1);

        write(1, "received: ", 10);
        write(1, buffer, n);

        n = sendto(fd, buffer, n, 0, (struct sockaddr*)&addr, addrlen);
        if (n == -1) /*error*/ exit(1);
    }
    freeaddrinfo(res);
    close(fd);
}

void handle_requests_tcp(char *port, bool verbose) {}
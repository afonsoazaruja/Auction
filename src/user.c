#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include "getIp.h"
#define DEFAULT_PORT "58023"

int main(int argc, char **argv) {
    int fd,errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char* default_ip;
    char buffer[128];
    char hostname[128];
    char port[6] = DEFAULT_PORT;
    char *asip = getIpAdress();
    
    // Update ip and/or port 
    if (argc > 1) {
        // ip included first
        if (!strcmp(argv[1], "-n")) {
            free(asip);
            memcpy(asip, argv[2], strlen(argv[2]) + 1);
            // ip and port included
            if (argc == 5 && !strcmp(argv[3], "-p")) 
                memcpy(port, argv[4], strlen(argv[4]) + 1);

        // port included first
        } else if (!strcmp(argv[1], "-p")) { 
            memcpy(port, argv[2], strlen(argv[2]) + 1);
            // port and ip included 
            if (argc == 5 && !strcmp(argv[3], "-n")) {
                free(asip);
                memcpy(asip, argv[4], strlen(argv[4]) + 1);
            } 
        }
    }
    // Print the results (everything tested and working as expected) PRINTS AND COMMENT TO BE DELETED
    printf("asip: %s\n", asip);
    printf("port: %s\n", port);


    fd=socket(AF_INET,SOCK_DGRAM,0); // UDP socket
    if(fd==-1) /*error*/exit(1);

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_DGRAM; //UDP socket

    errcode=getaddrinfo("tejo.tecnico.ulisboa.pt",DEFAULT_PORT,&hints,&res);
    if(errcode!=0) /*error*/ exit(1);

    n=sendto(fd, "Hello\n", 7, 0, res->ai_addr, res->ai_addrlen);
    if(n==-1) /*error*/ exit(1);

    addrlen=sizeof(addr);
    n=recvfrom(fd,buffer,128,0,
    (struct sockaddr*)&addr,&addrlen);
    if(n==-1) /*error*/ exit(1);
    
    write(1,"echo: ",6); write(1,buffer,n);
    free(asip);
    freeaddrinfo(res);
    close(fd);

 }
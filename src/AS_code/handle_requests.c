#include "handle_requests.h"
#include "aux_executes.h"
#include "executes.h"

void handle_requests_udp(char *port, bool verbose) {
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char request[128];

    fd = socket(AF_INET, SOCK_DGRAM, 0); 
    if (fd == -1) /*error*/ exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       
    hints.ai_socktype = SOCK_DGRAM;  
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(NULL, port, &hints, &res);
    if (errcode != 0) exit(1);

    n = bind(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) exit(1);

    while (true) {
        addrlen = sizeof(addr);
        n = recvfrom(fd, request, 128, 0, (struct sockaddr*)&addr, &addrlen);
        if (n == -1) exit(1);
        request[n] = '\0';

        printf("received: %s", request);
        if (!validate_buffer(request)) {
            send_reply_to_user(fd, addr, "ERR");
        } else {
            execute_request_udp(fd, addr, request);       
        }
    }
    freeaddrinfo(res);
    close(fd);
}

void execute_request_udp(int fd, struct sockaddr_in addr, char *request) {
    char cmd[CMD_SIZE + 1];
    sscanf(request, "%s", cmd);

    if (strcmp(cmd, "LIN") == 0)
        ex_login(fd, addr, request);
    else if (strcmp(cmd, "LOU") == 0)
        ex_logout(fd, addr, request);
    else if (strcmp(cmd, "UNR") == 0)
        ex_unregister(fd, addr, request);
    else if (strcmp(cmd, "LMA") == 0)
        ex_myauctions(fd, addr, request);
    else if (strcmp(cmd, "LMB") == 0)
        ex_mybids(fd, addr, request);
    else if (strcmp(cmd, "LST") == 0)
        ex_list(fd, addr, request);
    else if (strcmp(cmd, "SRC") == 0)
        ex_show_record(fd, addr, request);
    else 
        send_reply_to_user(fd, addr, "ERR\n");
}

void handle_requests_tcp(char *port, bool verbose) {
    int fd, errcode, newfd;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];
    char *request;

    fd = socket(AF_INET, SOCK_STREAM, 0);  // TCP socket
    if (fd == -1) /*error*/ exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM;  // TCP socket
    hints.ai_flags = AI_PASSIVE;

    errcode = getaddrinfo(NULL, port, &hints, &res);
    if (errcode != 0) /*error*/ exit(1);

    n = bind(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) /*error*/ exit(1);

    if(listen(fd,5)==-1)/*error*/exit(1);

    while (1) {
        addrlen = sizeof(addr);
        if((newfd=accept(fd,(struct sockaddr*)&addr, &addrlen))==-1)/*error*/exit(1);
        
        n=read(newfd, request, 128);
        if(n==-1)/*error*/exit(1);
        write(1,"received: ",10); write(1,buffer,n);

        n=write(newfd,buffer,n);
        if(n==-1)/*error*/exit(1);

        close(newfd);
    }
    freeaddrinfo(res);
    close(fd);
}


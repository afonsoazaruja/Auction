#include "handle_requests.h"
#include <stdio.h>

void handle_requests_udp(char *port, bool verbose) {
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char msg[128];

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
        n = recvfrom(fd, msg, 128, 0, (struct sockaddr*)&addr, &addrlen);
        if (n == -1) /*error*/ exit(1);

        printf("received: %s", msg);
        printf("new: %s", msg);
        
        execute_command_udp(fd, addr, msg);       
    }
    freeaddrinfo(res);
    close(fd);
}

void handle_requests_tcp(char *port, bool verbose) {
    int fd, errcode, newfd;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];
    char *msg;

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
        if((newfd=accept(fd,(struct sockaddr*)&addr, &addrlen))==-1) /*error*/ exit(1);
        
        n=read(newfd, msg, 128);
        if(n==-1)/*error*/exit(1);
        write(1,"received: ",10); write(1,buffer,n);

        n=write(newfd,buffer,n);
        if(n==-1)/*error*/exit(1);

        close(newfd);
    }
    freeaddrinfo(res);
    close(fd);
}

void execute_command_udp(int fd, struct sockaddr_in addr, char *msg) {
    char args[5][128];
    char cmd[10];
    sscanf(msg, "%s", cmd); // extract command

    if (!strcmp(cmd, "LIN")) 
        ex_login(fd, addr, msg);
    else if (!strcmp(cmd, "LOU")) 
        ex_logout(fd, addr, msg); 
    else if (!strcmp(cmd, "UNR")) 
        ex_unregister(fd, addr, msg); 
    else if (!strcmp(cmd, "LMA")) 
        ex_myauctions(fd, addr, msg); 
    else if (!strcmp(cmd, "LMB")) 
        ex_mybids(fd, addr, msg); 
    else if (!strcmp(cmd, "LST")) 
        ex_list(fd, addr, msg); 
    else if (!strcmp(cmd, "SRC"))
        ex_show_record(fd, addr, msg); 
}

void send_msg_to_user(int fd, struct sockaddr_in addr, char *msg) {
    int n = sendto(fd, msg, strlen(msg), 0, (struct sockaddr*)&addr, sizeof(addr));
    if (n == -1) /*error*/ exit(1);
}

void ex_login(int fd, struct sockaddr_in addr, char *msg) {
    // To do (incomplete)
    send_msg_to_user(fd, addr, "successful login\n");
}
void ex_logout(int fd, struct sockaddr_in addr, char *msg) {
    // To do
    send_msg_to_user(fd, addr, "TEST\n");
}
void ex_unregister(int fd, struct sockaddr_in addr, char *msg) {
    // To do
    send_msg_to_user(fd, addr, "TEST\n");
}
void ex_myauctions(int fd, struct sockaddr_in addr, char *msg) {
    // To do
    send_msg_to_user(fd, addr, "TEST\n");
}
void ex_mybids(int fd, struct sockaddr_in addr, char *msg) {
    // To do
    send_msg_to_user(fd, addr, "TEST\n");
}
void ex_list(int fd, struct sockaddr_in addr, char *msg) {
    // To do
    send_msg_to_user(fd, addr, "TEST\n");
}
void ex_show_record(int fd, struct sockaddr_in addr, char *msg) {
    // To do
    send_msg_to_user(fd, addr, "TEST\n");
}

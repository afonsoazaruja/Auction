#include "user.h"

int main(int argc, char **argv) {
    char port[6] = DEFAULT_PORT;
    char *asip = getIpAdress();
    char buffer[128];
    int socket_type;

    session user = {false, "NULL", "NULL"};
    
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
    // Print the results (everything tested and working as expected) PRINTS SHALL BE DELETED
    printf("asip: %s\n", asip);
    printf("port: %s\n", port);


    while (true) {
        fgets(buffer, sizeof(buffer), stdin);
        if (!is_input_valid(buffer, &socket_type, &user)) {
            printf("ERR: %s\n", buffer);
        } else {
            if (strcmp(buffer, "EXT\n") == 0) break;
            if (socket_type == SOCK_DGRAM) send_request_udp(port, asip, buffer);
            else if (socket_type == SOCK_STREAM) send_request_tcp(port, asip, buffer);
        }
    }
    free(asip);
}  

 void send_request_udp(char *port, char *asip, char *buffer) {
    int fd,errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    
    fd=socket(AF_INET,SOCK_DGRAM,0); 
    if(fd==-1) /*error*/exit(1);

    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    errcode=getaddrinfo(asip, port, &hints, &res);
    if(errcode!=0) /*error*/ exit(1);

    n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) /*error*/ exit(1);

    addrlen = sizeof(addr);
    n = recvfrom(fd, buffer, 128, 0, (struct sockaddr*)&addr, &addrlen);
    if (n == -1) /*error*/ exit(1);

    buffer[n] = '\0';
    analyze_reply(fd, buffer, n);
    write(1, buffer, strlen(buffer));

    freeaddrinfo(res);
    close(fd);
 }

void analyze_reply(int fd, char *buffer, int n) {
    char type_reply[4];
    sscanf(buffer, "%s", type_reply); 

    if (strcmp(type_reply, "RLI") == 0) { // reply for login
        char status[4];
        sscanf(buffer, "%*s %s", status);
        if (strcmp(status, "OK") == 0) {
            sprintf(buffer, "successful login\n");
        } else if (strcmp(status, "NOK") == 0) {
            sprintf(buffer, "incorrect login attempt\n");
        } else if (strcmp(status, "REG") == 0) {
            sprintf(buffer, "new user registered\n");
        }
    }
}

 void send_request_tcp(char *port, char *asip, char *buffer) {
    int fd, errcode;
    ssize_t n;
    struct addrinfo hints, *res;

    fd=socket(AF_INET,SOCK_STREAM,0); // TCP socket
    if (fd==-1) exit(1); //error

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket

    errcode=getaddrinfo(asip,port,&hints,&res);
    if(errcode!=0)/*error*/exit(1);

    n=connect(fd,res->ai_addr,res->ai_addrlen);
    if(n==-1)/*error*/exit(1);

    n=write(fd, buffer, strlen(buffer));
    if(n==-1)/*error*/exit(1);

    n=read(fd,buffer,128);
    if(n==-1)/*error*/exit(1);

    write(1,buffer,n);

    freeaddrinfo(res);
    close(fd);
}

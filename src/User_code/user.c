#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "getIp.h"
#include "verify_commands.h"
#define DEFAULT_PORT "58023"

int main(int argc, char **argv) {
    int fd,errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    char buffer[128];
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
    // Print the results (everything tested and working as expected) PRINTS SHALL BE DELETED
    printf("asip: %s\n", asip);
    printf("port: %s\n", port);

    fd=socket(AF_INET,SOCK_DGRAM,0); 
    if(fd==-1) /*error*/exit(1);

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; // IPv4
    hints.ai_socktype=SOCK_DGRAM; // UDP socket

    // Does not pass if you pick random ip and/or port. 
    errcode=getaddrinfo(asip, port, &hints, &res);
    if(errcode!=0) /*error*/ exit(1);

    while(true) {
        fgets(buffer, sizeof(buffer), stdin);
        
        char words_command[10][20] = {0};  
        int j = 0, k = 0;
        // extract words from command 
        for (int i=0; buffer[i]!='\n'; i++) {
            if (buffer[i] != ' ') {
                words_command[j][k] = buffer[i];
                k++;
            } else {
                words_command[j][k] = '\0';
                j++;
                k = 0;
            }
        }
        puts(words_command[0]);
        if (!is_command_valid(words_command)) {
            printf("Invalid command. Please verify your input.\n");
        } else {
            puts(words_command[0]);
            if (!strcmp(buffer, "exit\n")) break;

            n=sendto(fd,words_command, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
            if(n==-1) /*error*/ exit(1);

            addrlen=sizeof(addr);
            n=recvfrom(fd, buffer,128,0, (struct sockaddr*)&addr,&addrlen);
            if(n==-1) /*error*/ exit(1);
            
            write(1, buffer, n);
        }
    }
    free(asip);
    freeaddrinfo(res);
    close(fd);
 }  

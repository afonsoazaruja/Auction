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
void execute_command_udp(int fd, struct sockaddr_in addr, char words_command[10][20]);
void send_msg_to_user(int fd, struct sockaddr_in addr, char *message);
void ex_login(int fd, struct sockaddr_in addr, char words_command[10][20]);
void ex_logout(int fd, struct sockaddr_in addr, char words_command[10][20]);
void ex_unregister(int fd, struct sockaddr_in addr, char words_command[10][20]);
void ex_myauctions(int fd, struct sockaddr_in addr, char words_command[10][20]);
void ex_mybids(int fd, struct sockaddr_in addr, char words_command[10][20]);
void ex_list(int fd, struct sockaddr_in addr, char words_command[10][20]);
void ex_show_record(int fd, struct sockaddr_in addr, char words_command[10][20]);

#endif
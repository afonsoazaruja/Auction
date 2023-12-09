#include "handle_requests.h"
#include "aux_executes.h"
#include "executes.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/select.h>

#define DEFAULT_PORT "58023"
#define MAX_BUFFER_SIZE 128

void handle_requests_combined(char *port, bool verbose) {
    int udp_socket, tcp_socket;
    struct sockaddr_in udp_addr, tcp_addr;
    socklen_t udp_addrlen, tcp_addrlen;
    char buffer[MAX_BUFFER_SIZE];

    // Create UDP socket
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket == -1) {
        perror("UDP socket");
        exit(EXIT_FAILURE);
    }

    // Create TCP socket
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket == -1) {
        perror("TCP socket");
        exit(EXIT_FAILURE);
    }

    // Initialize UDP address structure
    memset(&udp_addr, 0, sizeof(udp_addr));
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = INADDR_ANY;
    udp_addr.sin_port = htons(atoi(port));

    // Initialize TCP address structure
    memset(&tcp_addr, 0, sizeof(tcp_addr));
    tcp_addr.sin_family = AF_INET;
    tcp_addr.sin_addr.s_addr = INADDR_ANY;
    tcp_addr.sin_port = htons(atoi(port));

    // Bind UDP socket
    if (bind(udp_socket, (struct sockaddr*)&udp_addr, sizeof(udp_addr)) == -1) {
        perror("UDP bind");
        exit(EXIT_FAILURE);
    }
    // Bind TCP socket
    if (bind(tcp_socket, (struct sockaddr*)&tcp_addr, sizeof(tcp_addr)) == -1) {
        perror("TCP bind");
        exit(EXIT_FAILURE);
    }
    // Listen for incoming TCP connections
    if (listen(tcp_socket, 5) == -1) {
        perror("TCP listen");
        exit(EXIT_FAILURE);
    }

    printf("port:%s\nverbose: %d\n", port, verbose);

    while (true) {
        fd_set read_fds;
        int max_fd;

        FD_ZERO(&read_fds);
        FD_SET(udp_socket, &read_fds);
        FD_SET(tcp_socket, &read_fds);

        max_fd = (udp_socket > tcp_socket) ? udp_socket : tcp_socket;

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }
        // Check UDP socket
        if (FD_ISSET(udp_socket, &read_fds)) {
            udp_addrlen = sizeof(struct sockaddr_in);
            ssize_t n = recvfrom(udp_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&udp_addr, &udp_addrlen);
            if (n == -1) perror("UDP recvfrom");
            else {
                buffer[n] = '\0';
                if(verbose) printf("UDP received: %s", buffer);
                if (!validate_buffer(buffer)) {
                   send_reply_to_user(udp_socket, udp_addr, "ERR");
                } else {
                    execute_request_udp(udp_socket, udp_addr, buffer);       
                }
            }
        }
        // Check TCP socket
        if (FD_ISSET(tcp_socket, &read_fds)) {
            int new_tcp_socket;

            tcp_addrlen = sizeof(struct sockaddr_in);
            new_tcp_socket = accept(tcp_socket, (struct sockaddr*)&tcp_addr, &tcp_addrlen);
            if (new_tcp_socket == -1) perror("TCP accept"); 
            else {
                ssize_t n = recv(new_tcp_socket, buffer, sizeof(buffer), 0);
                if (n == -1) {
                    perror("TCP recv");
                } else if (n == 0) {
                    close(new_tcp_socket); // Connection closed
                } else {
                    buffer[n] = '\0';
                    if(verbose) printf("TCP received: %s", buffer);
                    if(!validate_buffer(buffer)) send_reply_to_user(new_tcp_socket, tcp_addr, "ERR");
                    else execute_request_tcp(new_tcp_socket, tcp_addr, buffer);
                }
            }
        }
    }
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

void execute_request_tcp(int fd, struct sockaddr_in addr, char *request) {
    char cmd[CMD_SIZE + 1];
    sscanf(request, "%s", cmd);

    if (strcmp(cmd, "OPA") == 0)
        ex_open(fd, addr, request);
    else if (strcmp(cmd, "CLS") == 0)
        ex_close(fd, addr, request);
    else if (strcmp(cmd, "SAS") == 0)
        ex_show_asset(fd, addr, request);
    else if (strcmp(cmd, "BID") == 0)
        ex_bid(fd, addr, request);
    else 
        send_reply_to_user(fd, addr, "ERR\n");
}


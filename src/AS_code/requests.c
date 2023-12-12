#include "requests.h"
#include "aux_executes.h"
#include <stdio.h>
#include <sys/select.h>

void handle_requests(char *port) {
    int udp_socket, tcp_socket;
    struct sockaddr_in udp_addr, tcp_addr;
    socklen_t udp_addrlen, tcp_addrlen;
    char buffer[MAX_BUFFER_SIZE];

    udp_socket = do_socket(SOCK_DGRAM);
    tcp_socket = do_socket(SOCK_STREAM);

    initialize_addr(&udp_addr, port);
    initialize_addr(&tcp_addr, port);

    do_bind(udp_socket, &udp_addr);
    do_bind(tcp_socket, &tcp_addr);

    if (listen(tcp_socket, 5) == -1) {
        perror("TCP listen");
        exit(1);
    }

    while (true) {
        fd_set read_fds;
        int max_fd;

        FD_ZERO(&read_fds);
        FD_SET(udp_socket, &read_fds);
        FD_SET(tcp_socket, &read_fds);

        max_fd = (udp_socket > tcp_socket) ? udp_socket : tcp_socket;
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(1);
        }
        // Check UDP socket
        if (FD_ISSET(udp_socket, &read_fds)) {
            handle_udp_socket(udp_socket, udp_addr);
        }
        // Check TCP socket
        if (FD_ISSET(tcp_socket, &read_fds)) {
            handle_tcp_socket(tcp_socket, tcp_addr);
        }
    }
}

int do_socket(int socket_type) {
    int s;
    s = socket(AF_INET, socket_type, 0);
    if (s == -1) {
        if (socket_type == SOCK_DGRAM) perror("UDP socket");
        else if (socket_type == SOCK_STREAM) perror("TCP socket");
        puts("ERROR");
        exit(1);
    }
    return s;
}

void initialize_addr(struct sockaddr_in *addr, char *port) {
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(atoi(port));
}

void do_bind(int socket, struct sockaddr_in *addr) {
   if (bind(socket, (struct sockaddr*)addr, sizeof(*addr)) == -1) {
        perror("bind error");
        exit(1);
    }
}

void handle_udp_socket(int udp_socket, struct sockaddr_in udp_addr) {
    socklen_t udp_addrlen;
    char buffer[MAX_BUFFER_SIZE];

    udp_addrlen = sizeof(struct sockaddr_in);
    ssize_t n = recvfrom(udp_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&udp_addr, &udp_addrlen);
    
    if (n == -1) {
        perror("UDP recvfrom");
    } else {
        buffer[n] = '\0';
        if (verbose) printf("UDP received: %s", buffer);

        if (!validate_buffer(buffer)) {
            send_reply_to_user(udp_socket, udp_addr, "ERR\n");
        } else {
            execute_request_udp(udp_socket, udp_addr, buffer);
        }
    }
}

void handle_tcp_socket(int tcp_socket, struct sockaddr_in tcp_addr) {
    socklen_t tcp_addrlen;
    char buffer[MAX_BUFFER_SIZE];

    int new_tcp_socket;
    tcp_addrlen = sizeof(struct sockaddr_in);
    new_tcp_socket = accept(tcp_socket, (struct sockaddr*)&tcp_addr, &tcp_addrlen);

    if (new_tcp_socket == -1) {
        perror("TCP accept");
    }
    else {
        ssize_t n, total = 0;
        while (total < 3) { // cmd
            n = recv(new_tcp_socket, buffer + total, 1, 0);
            if(n==-1)/*error*/exit(EXIT_FAILURE);
            total += n;
        }
        buffer[total] = '\0';
        execute_request_tcp(new_tcp_socket, tcp_addr, buffer);
        close(new_tcp_socket);
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
    if (strcmp(cmd, "OPA") == 0) {
        extract(request, fd, 8);
        if (verbose) printf("TCP received: %s\n", request);
        ex_open(fd, addr, request);
    } else {
        extract(request, fd, 0);
        if (verbose) printf("TCP received: %s", request);

        if (!validate_buffer(request)) send_reply_to_user(fd, addr, "ERR\n");
        if (strcmp(cmd, "CLS") == 0)
            ex_close(fd, addr, request);
        else if (strcmp(cmd, "SAS") == 0)
            ex_show_asset(fd, addr, request);
        else if (strcmp(cmd, "BID") == 0)
            ex_bid(fd, addr, request);
        else
            send_reply_to_user(fd, addr, "ERR\n");
    }
}

void extract(char *src, int fd, int args) {
    int num_spaces = 0;
    ssize_t n = 0, total = 3; // 3 for cmd
    while(args == 0 || args > num_spaces) {
        n=recv(fd, src + total, 1, 0);
        if(n==-1)/*error*/exit(EXIT_FAILURE);
        total += n;
        src[total] = '\0';
        if (src[total-1] == ' ') {
            if(++num_spaces == args) {
                src[total-1] = '\0';
                break;
            }
        }
        if (src[total-1] == '\n' || src[total-1] == '\0') {
            src[total] = '\0';
            break;
        }
    }
}

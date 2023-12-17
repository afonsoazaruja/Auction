#include "requests.h"
#include "aux_executes.h"
#include <sys/types.h>

volatile sig_atomic_t ctrl_c = 0;

void handle_requests(char *port) {
    int udp_socket, tcp_socket;
    struct sockaddr_in udp_addr, tcp_addr;

    udp_socket = do_socket(SOCK_DGRAM);
    tcp_socket = do_socket(SOCK_STREAM);

    initialize_addr(&udp_addr, port);
    initialize_addr(&tcp_addr, port);

    // set_timeout(udp_socket, SO_RCVTIMEO);
    // set_timeout(tcp_socket, SO_RCVTIMEO);

    do_bind(udp_socket, &udp_addr);
    do_bind(tcp_socket, &tcp_addr);

    if (listen(tcp_socket, 5) == -1) {
        perror("TCP listen"); exit(1);
    }

    if (signal(SIGINT, handle_SIGINT) == SIG_ERR) {
        perror("Error setting up signal handler SIGINT"); exit(1);
    }
    if (signal(SIGCHLD, handle_SIGCHLD) == SIG_ERR) {
        perror("Error setting up signal handler SIGCHLD"); exit(1);
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork protocols"); exit(1);
    }
    while (true) {
        if (ctrl_c == 1) break;
        // Check UDP socket (child)
        if (pid == 0) handle_udp_socket(udp_socket, udp_addr);
        // Check TCP socket (parent)
        else handle_tcp_socket(tcp_socket, tcp_addr);
    }
    if (pid == 0) exit(0);
    else exit(0);
}

int do_socket(int socket_type) {
    int s;
    s = socket(AF_INET, socket_type, 0);
    if (s == -1) {
        if (socket_type == SOCK_DGRAM) perror("UDP socket");
        else if (socket_type == SOCK_STREAM) perror("TCP socket");
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
        perror("Bind error"); exit(1);
    }
}

void handle_udp_socket(int udp_socket, struct sockaddr_in udp_addr) {
    socklen_t udp_addrlen;
    char buffer[MAX_BUFFER_SIZE];

    udp_addrlen = sizeof(struct sockaddr_in);
    ssize_t n = recvfrom(udp_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)
    &udp_addr, &udp_addrlen);
    if (n == -1) {
        if (errno == EINTR) {
            return;
        }
        perror("UDP recvfrom"); return;
    }
    else {
        buffer[n] = '\0';
        if (verbose) printf("> UDP received: %s", buffer);

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
        if (errno == EINTR) {
            return;
        }
        perror("TCP accept");
        exit(1);
    }
    else {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork tcp"); close(new_tcp_socket); return;
        }
        if (pid == 0) { // child
            ssize_t n, total = 0;
            while (total < 3) { // cmd
                n = recv(new_tcp_socket, buffer + total, 1, 0);
                if(n==-1) {
                    close(new_tcp_socket); exit(1);
                }
                total += n;
            }
            buffer[total] = '\0';
            execute_request_tcp(new_tcp_socket, tcp_addr, buffer);
            close(new_tcp_socket);
            exit(0); // child process terminates
        } 
        // parent continues
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
        // read everything except file bytes (8 spaces)
        if (read_request_tcp(request, fd, 8) == -1) return;
        strcat(request, "\n");
        if (!validate_buffer(request)) send_reply_to_user(fd, addr, "ERR\n");
        if (verbose) printf("> TCP received: %s", request);
        ex_open(fd, addr, request);
    } else {
        // read everything
        if(read_request_tcp(request, fd, 0) == -1) return;
        if (!validate_buffer(request)) send_reply_to_user(fd, addr, "ERR\n");
        if (verbose) printf("> TCP received: %s", request);

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

int read_request_tcp(char *src, int fd, int spaces) {
    int num_spaces = 0;
    ssize_t n = 0, total = 3; // 3 for cmd
    while(spaces == 0 || spaces > num_spaces) {
        n=recv(fd, src + total, 1, 0);
        if(n==-1) {
            perror("Error in recv"); return -1;
        }
        total += n;
        src[total] = '\0';
        if (src[total-1] == ' ') {
            if(++num_spaces == spaces) {
                src[total-1] = '\0';
                break;
            }
        }
        if (src[total-1] == '\n' || src[total-1] == '\0') {
            src[total] = '\0';
            break;
        }
    }
    return 0;
}

void handle_SIGINT(int SIGNAL) {
    ctrl_c = 1;
    exit(0);  
}

void handle_SIGCHLD(int SIGNAL) {
    while(waitpid(0, NULL, WNOHANG) > 0);
}

void set_timeout(int fd, int optname) {
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;

    if (setsockopt(fd, SOL_SOCKET, optname, (const char*)&timeout, sizeof(timeout)) < 0) {
        perror("Error setting socket options"); close(fd); exit(1);
    }
}
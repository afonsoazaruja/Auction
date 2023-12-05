#include "user.h"
#include "replies.h"
//#include "getIp.h"

session user = {false, "", ""};

int main(int argc, char **argv) {
    int socket_type;
    char port[6] = DEFAULT_PORT;
    char *asip = getIpAddress();
    char buffer[BUFFER_SIZE + 1];
    //char *buffer = (char*)malloc(BUFFER_SIZE + 1);
    
    // Update ip and/or port 
    if (argc > 1) {
        // ip included first
        if (!strcmp(argv[1], "-n")) {
            memset(asip, 0, INET_ADDRSTRLEN);
            memcpy(asip, argv[2], strlen(argv[2]) + 1);
            // ip and port included
            if (argc == 5 && !strcmp(argv[3], "-p")) 
                memcpy(port, argv[4], strlen(argv[4]) + 1);

        // port included first
        } else if (!strcmp(argv[1], "-p")) { 
            memcpy(port, argv[2], strlen(argv[2]) + 1);
            // port and ip included 
            if (argc == 5 && !strcmp(argv[3], "-n")) {
                memset(asip, 0, INET_ADDRSTRLEN);
                memcpy(asip, argv[4], strlen(argv[4]) + 1);
            } 
        }
    }
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        fgets(buffer, BUFFER_SIZE, stdin);
        if (!is_input_valid(buffer, &socket_type, &user)) {
            printf("ERR: %s\n", buffer);
        } else {
            if (strcmp(buffer, "EXT\n") == 0) break;
            if (socket_type == SOCK_DGRAM)
                send_request_udp(port, asip, buffer);
            else if (socket_type == SOCK_STREAM)
                send_request_tcp(port, asip, buffer);
        }
    }
    free(asip);
}

void send_request_tcp(char *port, char *asip, char *buffer) {
    char cmd[4];
    int fd, errcode;
    ssize_t n;
    // socklen_t addrlen;
    struct addrinfo hints, *res;
    //struct sockaddr_in addr;

    fd=socket(AF_INET,SOCK_STREAM,0); // TCP socket
    if (fd==-1) exit(1); //error

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket

    errcode=getaddrinfo(asip,port,&hints,&res);
    if(errcode!=0)/*error*/exit(1);

    n=connect(fd,res->ai_addr,res->ai_addrlen);
    if(n==-1)/*error*/exit(1);

    sscanf(buffer, "%s", cmd);
    if (strcmp(cmd, "OPA") == 0) {
        char name[MAX_NAME_DESC + 1];
        char start_value[MAX_START_VAL + 1];
        char timeactive[MAX_AUC_DURATION + 1];
        char asset_fname[MAX_FILENAME];
        long size = 0;

        sscanf(buffer, "%*s %s %s %s %s %ld", name, start_value, timeactive, asset_fname, &size);

        int asset_fd = open(asset_fname, O_RDONLY);
        if (asset_fd == -1) {
            perror("Error opening file");
            exit(1);
        }
        char *fname = get_file_name(asset_fname);
        
        sprintf(buffer, "OPA %s %s %s %s %s %s %ld ", user.UID, user.password, name, start_value, timeactive, fname, size);    
        free(fname);

        n=write(fd, buffer, strlen(buffer));
        if(n==-1)/*error*/exit(1);
        
        off_t offset = 0;
        while (size > 0) {
            ssize_t sent_bytes = sendfile(fd, asset_fd, &offset, size);
            if (sent_bytes == -1) {
                perror("Error sending file");
                exit(1);
            }
            size -= sent_bytes;
        }
        n=write(fd, "\n", 1);
        if(n==-1)/*error*/exit(1);
        close(asset_fd);
    } else {
        n=write(fd, buffer, strlen(buffer));
        if(n==-1)/*error*/exit(1);
    }

    n=read(fd,buffer, BUFFER_SIZE);
    if(n==-1)/*error*/exit(1);
    buffer[n] = '\0';
    
    write(1,buffer,n);

    freeaddrinfo(res);
    close(fd);
}

void send_request_udp(char *port, char *asip, char *buffer) {
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    
    fd = socket(AF_INET, SOCK_DGRAM, 0); 
    if (fd == -1) /*error*/exit(1);

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; // IPv4
    hints.ai_socktype=SOCK_DGRAM; // UDP socket
    errcode = getaddrinfo(asip, port, &hints, &res);
    if (errcode != 0)/*error*/ exit(1);

    n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) /*error*/ exit(1);

    addrlen = sizeof(addr);

    n = recvfrom(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&addr, &addrlen);
    if (n == -1) exit(1);
    buffer[n] = '\0';

    analyze_reply_udp(buffer);
    write(1, buffer, strlen(buffer));
    freeaddrinfo(res);
    close(fd);
}

char* getIpAddress() {
    char hostname[128];
    extern int errno;
    struct addrinfo hints,*res,*p;
    int errcode;
    struct in_addr *addr;
    char* ipAddress = malloc(INET_ADDRSTRLEN);  

    if(gethostname(hostname,128)==-1) {
        fprintf(stderr,"error: %s\n",strerror(errno));
        exit(EXIT_FAILURE); 
    }
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_CANONNAME;

    if ((errcode=getaddrinfo(hostname,NULL,&hints,&res))!=0) {
        fprintf(stderr,"error: getaddrinfo: %s\n",gai_strerror(errcode));
        exit(EXIT_FAILURE);
    } else {
        for(p=res;p!=NULL;p=p->ai_next){
            addr=&((struct sockaddr_in *)p->ai_addr)->sin_addr;
            inet_ntop(p->ai_family, addr, ipAddress, INET_ADDRSTRLEN);
        }
        freeaddrinfo(res);
    }
    return ipAddress;
}








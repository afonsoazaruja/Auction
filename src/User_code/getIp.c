#include "getIp.h"

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

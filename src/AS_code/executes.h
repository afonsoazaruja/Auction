#include "handle_requests.h"

void ex_login(int fd, struct sockaddr_in addr, char *request);
void ex_logout(int fd, struct sockaddr_in addr, char* msg);
void ex_unregister(int fd, struct sockaddr_in addr, char* msg);
void ex_myauctions(int fd, struct sockaddr_in addr, char* msg);
void ex_mybids(int fd, struct sockaddr_in addr, char* msg);
void ex_list(int fd, struct sockaddr_in addr, char* msg);
void ex_show_record(int fd, struct sockaddr_in addr, char* msg);
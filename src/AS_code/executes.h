#include "requests.h"
#include "as.h"

#define USERS_DIR "../../ASDIR/USERS"
#define AUCTIONS_DIR "../../ASDIR/AUCTIONS"

void ex_login(int fd, struct sockaddr_in addr, char *request);
void ex_logout(int fd, struct sockaddr_in addr, char* msg);
void ex_unregister(int fd, struct sockaddr_in addr, char* msg);
void ex_myauctions(int fd, struct sockaddr_in addr, char* msg);
void ex_mybids(int fd, struct sockaddr_in addr, char* msg);
void ex_list(int fd, struct sockaddr_in addr, char* msg);
void ex_show_record(int fd, struct sockaddr_in addr, char* msg);
void ex_open(int fd, struct sockaddr_in addr, char *request);
void ex_close(int fd, struct sockaddr_in addr, char *request);
void ex_show_asset(int fd, struct sockaddr_in addr, char *request);
void ex_bid(int fd, struct sockaddr_in addr, char *request);
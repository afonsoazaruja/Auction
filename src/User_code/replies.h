#ifndef REPLIES_H
#define REPLIES_H

#include "user.h"

extern session user;

#define TYPE_REPLY_SIZE 3
#define MAX_STATUS_SIZE 3
#define BORDER_TOP_LEFT "┌"
#define BORDER_LEFT "│"
#define BORDER_BOTTOM_LEFT "└"
#define BORDER_HORIZONTAL "─"

void analyze_reply_udp(char *buffer);
void handle_auctions(char *list, char *buffer, char *type);
void reply_login(char *status, char *buffer);
void reply_logout(char *status, char *buffer);
void reply_unregister(char *status, char *buffer);
void reply_myauctions(char *status, char *buffer);
void reply_mybids(char *status, char *buffer);
void reply_list(char *status, char *buffer);
void reply_show_record(char *status, char *buffer, const char *list);

void analyze_reply_tcp(char *buffer, int fd);
void reply_open(char *status, char *buffer, int fd);
void reply_close(char *status, char *buffer);
void reply_show_asset(char *status, char *buffer, int fd);
void reply_bid(char *status, char *buffer);

void extract(char *src, char *dst, int fd);

#endif

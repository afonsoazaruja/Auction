#ifndef REPLIES_H
#define REPLIES_H

#include "user.h"

extern session user;

void analyze_reply_udp(char *buffer);
void get_ongoing_auctions(char *list, char *buffer, char *type);
void reply_login(char *status, char *buffer);
void reply_logout(char *status, char *buffer);
void reply_unregister(char *status, char *buffer);
void reply_myauctions(char *status, char *buffer);
void reply_mybids(char *status, char *buffer);
void reply_list(char *status, char *buffer);
void reply_show_record(char *status, char *buffer, const char *list);

void analyze_reply_tcp(char *buffer);
void reply_open(char *status, char *buffer);
void reply_close(char *status, char *buffer);
void reply_show_asset(char *status, char *buffer);
void reply_bid(char *status, char *buffer);

#endif

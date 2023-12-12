#ifndef VERIFY_COMMANDS_H
#define VERIFY_COMMANDS_H

#include "../validations.h"
#include <sys/stat.h>

#define MAX_CMD_SIZE 11

typedef struct session {
    bool logged;
    char UID[10];
    char password[10];
} session;

bool is_input_valid(char *buffer, int *socket_type, struct session *user);
bool handle_login(char *buffer, struct session *user);
bool handle_record(char *buffer);
bool handle_open(char *buffer, struct session *user);
bool handle_bid(char *buffer, struct session *user);
bool handle_close(char *buffer, struct session *user);
bool handle_asset(char *buffer);
long get_file_size(char *filename);
char *get_file_data(char *filename, long size);
char *get_file_name(char *dir);

#endif

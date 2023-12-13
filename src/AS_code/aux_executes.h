#include "executes.h"
#include <sys/stat.h>
#include "as.h"
#include <bits/types/cookie_io_functions_t.h>
#include <netinet/in.h>
#include <time.h>
#include <dirent.h>

void register_auction(int fd, struct sockaddr_in addr, char *uid, char *name, char *asset, char *start_value, char *timeactive, char *aid);
void try_to_login(int fd, struct sockaddr_in addr, char *uid, char *password);
void register_user(int fd, struct sockaddr_in addr, char *uid, char *password);
bool is_correct_password(char *password, char *uid);
void send_reply_to_user(int fd, struct sockaddr_in addr, char *message);
char *get_path_user_dir(char *path_user_dir, char *uid);
char *get_aid();
bool is_registered(char *uid);
bool is_logged_in(char *uid);
bool is_auction_active(char *aid);
bool is_auction_owned(char *uid, char *aid);
bool directoryExists(const char *path);
void create_login_file(char *uid);
void create_pass_file(char *uid, char *password);
void create_start_auction_file(char *uid, char *name, char *asset, char *start_value, char *timeactive, char *aid);
char *get_file_name(char *path_user_dir, char *first, char *second, char *third);

#include "executes.h"
#include <sys/stat.h>

void build_path_user_dir(char *path_user_dir, char *uid);
bool is_correct_password(char *password, char *path_user_dir, char *uid);
void send_reply_to_user(int fd, struct sockaddr_in addr, char *message);
char *get_path_user_dir(char *path_user_dir, char *uid);
bool is_registered(char *path_user_dir, char *uid);
bool is_logged_in(char *path_user_dir, char *uid);
bool directoryExists(const char *path);
void create_login_file(char *uid, char *path_user_dir);
void create_pass_file(char *uid, char *password, char *path_user_dir);
char *get_file_name(char *uid, char *path_user_dir, char *end_part);

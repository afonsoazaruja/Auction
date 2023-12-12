#include "executes.h"
#include <sys/stat.h>
#include "as.h"

void build_path_user_dir(char *path_user_dir, char *uid);
void build_path_auction_dir(char *path_auction_dir, char *aid);
void build_auction_asset_dir(char *path_auction_dir, char *aid);
void build_auction_bids_dir(char *path_auction_dir, char *aid);
void register_auction(int fd, struct sockaddr_in addr, char *uid, char *name, char *asset, char *start_value, char *timeactive, char *aid);
void try_to_login(int fd, struct sockaddr_in addr, char *path_user_dir, char *uid, char *password);
void register_user(int fd, struct sockaddr_in addr, char *path_user_dir, char *uid, char *password);
bool is_correct_password(char *password, char *path_user_dir, char *uid);
void send_reply_to_user(int fd, struct sockaddr_in addr, char *message);
char *get_path_user_dir(char *path_user_dir, char *uid);
bool is_registered(char *path_user_dir, char *uid);
bool is_logged_in(char *path_user_dir, char *uid);
bool directoryExists(const char *path);
void create_login_file(char *uid, char *path_user_dir);
void create_pass_file(char *uid, char *password, char *path_user_dir);
void create_start_auction_file(char *auction_dir, char *uid, char *name, char *asset, char *start_value, char *timeactive, char *aid);
char *get_file_name(char *path_user_dir, char *first, char *second, char *third);

void format_aid(char *aid);
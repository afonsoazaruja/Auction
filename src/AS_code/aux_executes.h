#include "executes.h"
#include "../validations.h"
#include <sys/stat.h>
#include "as.h"
#include <bits/types/cookie_io_functions_t.h>
#include <netinet/in.h>
#include <time.h>
#include <dirent.h>
#include <linux/limits.h>
#include <stdio.h>

#define PATH_USERS_DIR "../../ASDIR/USERS"
#define PATH_AUCTIONS_DIR "../../ASDIR/AUCTIONS"

typedef struct {
    char aid[4];
    int state;
} Auction;

void send_reply_to_user(int fd, struct sockaddr_in addr, char *reply);

bool is_correct_password(char *password, char *uid);
bool is_registered(char *uid);
bool is_logged_in(char *uid);
bool is_auction_active(char *aid);
bool is_auction_owned(char *uid, char *aid);
bool is_auct_hosted_by_user(char *aid, char *uid);
bool is_bid_too_small(char *aid, int bid_value);
bool directoryExists(const char *path);
bool has_started_auctions(char *uid);
bool has_placed_bids(char *uid);

void try_to_login(int fd, struct sockaddr_in addr, char *uid, char *password);
void register_user(int fd, struct sockaddr_in addr, char *uid, char *password);
void register_auction(int fd, struct sockaddr_in addr, char *uid, char *name, char *asset, char *start_value, char *timeactive, char *aid);
void close_auction(char *aid);

void create_login_file(char *uid);
void create_pass_file(char *uid, char *password);
void create_start_auction_file(char *uid, char *name, char *asset, char *start_value, char *timeactive, char *aid);
void create_bid(char *aid, char *uid, int value);
char* create_list_auctions(char *path_dir);

char *get_aid();
Auction* get_auctions(const char *path_dir, size_t num_auctions);
size_t get_num_files_in_dir(const char *path);
long get_file_size(char *fname);
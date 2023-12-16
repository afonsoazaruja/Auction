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

#define PATH_USERS_DIR "../ASDIR/USERS"
#define PATH_AUCTIONS_DIR "../ASDIR/AUCTIONS"

// INCORRECT FOR NOW
#define SIZE_BIDS_PATH 100

#define MAX_DIGITS_SEC_TIME 5
#define SIZE_DATE 10
#define SIZE_TIME 8

#define SIZE_END_FNAME 38
#define SIZE_START_FNAME 40
// INCORRECT FOR NOW
#define MAX_SIZE_START_FILE 100
#define MAX_SIZE_END_FILE 27

#define SIZE_DATE 10
#define SIZE_TIME 8
#define SIZE_BIDS_FNAME 10

typedef struct {
    char aid[SIZE_AID+1];
    int state;
} Auction;

typedef struct {
    char uid[SIZE_UID+1];
    int value;
    char date[SIZE_DATE+1];
    char time[SIZE_TIME+1];
    int sec_time;
} Bid;

void send_reply_to_user(int fd, struct sockaddr_in addr, char *reply);
void send_myauctions(int fd, struct sockaddr_in addr, char *uid); 
void send_mybids(int fd, struct sockaddr_in addr, char *uid);
void send_all_auctions(int fd, struct sockaddr_in addr, char* auctions_list);
void send_record(int fd, struct sockaddr_in addr, char *aid);
void send_asset(int fd, struct sockaddr_in addr, char *aid);
void receive_asset(int fd, struct sockaddr_in addr, char *aid, char *asset_fname, long size);

bool is_registered(char *uid);
bool is_logged_in(char *uid);
bool is_auction_owned(char *uid, char *aid);
bool is_auction_active(char *aid);
bool is_correct_password(char *password, char *uid);
bool is_bid_too_small(char *aid, int bid_value);
bool auction_exists(char *aid);
bool directoryExists(const char *path);
bool has_started_auctions(char *uid);
bool has_placed_bids(char *uid);
bool end_file_exists(char *aid);
bool file_exists(const char *filename);

void try_to_login(int fd, struct sockaddr_in addr, char *uid, char *password);
void logout(char *login_file_name, int fd, struct sockaddr_in addr, char *uid);
void unregister(char *login_file_name, char *pass_file_name, int fd, struct sockaddr_in addr, char *uid);

void register_user(int fd, struct sockaddr_in addr, char *uid, char *password);
void register_auction(int fd, struct sockaddr_in addr, char *uid, char *name, char *asset,
char *start_value, char *timeactive, char *aid);
void start_auction(char *uid, char *name, char *asset, char *start_value, char *timeactive, char *aid);
void close_auction(char *aid, long start_fulltime, long end_fulltime);

void create_login_file(char *uid);
void create_pass_file(char *uid, char *password);
void create_bid(char *aid, char *uid, int value);
void create_bids_file(char *aid, char *uid, int value);
void create_bidded_file(char *aid, char *uid);
char* create_list_auctions(char *path_dir);
char* create_list_bids(char *aid);
char* create_record(char *aid);

size_t get_num_auctions(const char *path);
size_t get_num_bids(const char *aid);
char *get_end_info(char *aid);
char* get_auction_info(char *aid);
Auction* get_auctions(const char *path_dir, size_t num_auctions);
Bid* get_bids(char *aid, size_t num_bids);
long get_file_size(char *fname);
int get_num_digits(int value);
char* get_aid();
char* get_asset_name(char *aid);
time_t get_start_fulltime(char *aid);

int compare_auctions(const void *a, const void *b);
int compare_bids(const void *a, const void *b);

void filtrate_info(char *auction_info);
void add_bid_to_list(Bid *bids, char *aid, char *bid_fname, int index);
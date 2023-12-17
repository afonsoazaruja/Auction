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

#define PATH_USERS              "../ASDIR/USERS"
#define SIZE_USER_PATH          14 + SIZE_UID + 1 // PATH_USERS (14), / (1)
#define SIZE_PASS_FILE_PATH     SIZE_USER_PATH + SIZE_UID + 9 + 1 // "_pass.txt" (9) + / (1)
#define SIZE_LOGIN_FILE_PATH    SIZE_USER_PATH + SIZE_UID + 10 + 1 // "_login.txt" (9) + / (1)
#define SIZE_BIDDED_PATH        SIZE_USER_PATH + 6 + 1 // "BIDDED" (6), / (1)
#define SIZE_BIDDED_FILE_PATH   SIZE_BIDDED_PATH + SIZE_AID + 4 + 1 // ".txt" (4), / (1)
#define SIZE_HOSTED_PATH        SIZE_USER_PATH + 6 + 1 // "HOSTED" (6), / (1)
#define SIZE_HOSTED_FILE_PATH   SIZE_HOSTED_PATH + SIZE_AID + 4 + 1 // ".txt" (4), / (1)

#define PATH_AUCTIONS           "../ASDIR/AUCTIONS"
#define SIZE_AUCTION_PATH       17 + SIZE_AID + 1 // PATH_AUCTION (17) + / (1)
#define SIZE_START_FILE_PATH    SIZE_AUCTION_PATH + 6 + SIZE_AID + 4 + 1 // "START_" (6), ".txt" (4), / (1)
#define SIZE_END_FILE_PATH      SIZE_AUCTION_PATH + 4 + SIZE_AID + 4 + 1 // "END_" (4), ".txt" (4), / (1) 
#define SIZE_BIDS_PATH          SIZE_AUCTION_PATH + 4 + 1 // "BIDS" (4), / (1)
#define SIZE_BIDS_FILE_PATH     SIZE_BIDS_PATH + SIZE_BIDS_FNAME + 1 // 000000.txt, / (1)
#define SIZE_ASSET_FILE_PATH    SIZE_AUCTION_PATH + 5 + MAX_FILENAME + 2 // "ASSET" (5), / (2)

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

int send_reply_to_user(int fd, struct sockaddr_in addr, char *reply);
int send_myauctions(int fd, struct sockaddr_in addr, char *uid); 
int send_mybids(int fd, struct sockaddr_in addr, char *uid);
int send_all_auctions(int fd, struct sockaddr_in addr, char* auctions_list);
int send_record(int fd, struct sockaddr_in addr, char *aid);
int send_asset(int fd, struct sockaddr_in addr, char *aid);
int receive_asset(int fd, struct sockaddr_in addr, char *aid, char *asset_fname, long size);

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
int logout(char *login_file_name, int fd, struct sockaddr_in addr, char *uid);
int unregister(char *login_file_name, char *pass_file_name, int fd, struct sockaddr_in addr, char *uid);

int register_user(int fd, struct sockaddr_in addr, char *uid, char *password);
int register_auction(int fd, struct sockaddr_in addr, char *uid, char *name, char *asset,
char *start_value, char *timeactive, char *aid);
int start_auction(char *uid, char *name, char *asset, char *start_value, char *timeactive, char *aid);
int close_auction(char *aid, long start_fulltime, long end_fulltime);

int create_login_file(char *uid);
int create_pass_file(char *uid, char *password);
int create_bid(char *aid, char *uid, int value);
int create_bids_file(char *aid, char *uid, int value);
int create_bidded_file(char *aid, char *uid);
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
int add_bid_to_list(Bid *bids, char *aid, char *bid_fname, int index);
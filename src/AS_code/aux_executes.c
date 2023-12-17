#include "aux_executes.h"
#include <dirent.h>
#include <stdio.h>

int send_reply_to_user(int fd, struct sockaddr_in addr, char *reply) {
    if (sendto(fd, reply, strlen(reply), 0,
    (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("[ERR] send_reply_to_user sendto");
        return -1;
    }
    if (verbose) printf("> Replied: %s", reply);
    return 0;
}

int send_myauctions(int fd, struct sockaddr_in addr, char *uid) {
    char hosted_path[SIZE_HOSTED_PATH+1];
    sprintf(hosted_path, "%s/%s/HOSTED", PATH_USERS, uid);
    char *auctions_list = create_list_auctions(hosted_path);
    if (auctions_list == NULL) return -1;

    char reply[strlen(auctions_list) + 10];
    sprintf(reply, "RMA OK %s\n", auctions_list);
    free(auctions_list);
    return send_reply_to_user(fd, addr, reply);
}

int send_mybids(int fd, struct sockaddr_in addr, char *uid) {
    char bidded_path[SIZE_BIDDED_PATH+1];
    sprintf(bidded_path, "%s/%s/BIDDED", PATH_USERS, uid);
    char *auctions_list = create_list_auctions(bidded_path);
    if (auctions_list == NULL) return -1;

    char reply[strlen(auctions_list) + 10];
    sprintf(reply, "RMB OK %s\n", auctions_list);
    free(auctions_list);
    return send_reply_to_user(fd, addr, reply);
}

int send_all_auctions(int fd, struct sockaddr_in addr, char *auctions_list) {
    char reply[strlen(auctions_list) + 10];
    sprintf(reply, "RLS OK %s\n", auctions_list);
    free(auctions_list);
    return send_reply_to_user(fd, addr, reply);
}

int send_record(int fd, struct sockaddr_in addr, char *aid) {
    char* record = create_record(aid);
    if (record == NULL) {
        return -1;
    }
    char reply[strlen(record) + 10];
    sprintf(reply, "RRC OK %s\n", record);
    free(record);
    return send_reply_to_user(fd, addr, reply);
}

int send_asset(int fd, struct sockaddr_in addr, char *aid) {
    char asset_path[SIZE_ASSET_FILE_PATH+1], reply[MAX_BUFFER_SIZE];
    ssize_t size = 0;
    char *asset_fname = get_asset_name(aid);
    if (asset_fname == NULL) return -1;

    sprintf(asset_path, "%s/%s/ASSET/%s", PATH_AUCTIONS, aid, asset_fname);
    size = get_file_size(asset_path);
    if (size == -1) { 
        free(asset_fname); 
        return -1; 
    }
    int asset_fd = open(asset_path, O_RDONLY);
    if (asset_fd == -1) {
        free(asset_fname);
        return -1;
    }
    sprintf(reply, "RSA OK %s %ld ", asset_fname, size);
    if (send_reply_to_user(fd, addr, reply) == -1) {
        free(asset_fname);
        return -1;
    }
    off_t offset = 0;
    while (size > 0) {
        ssize_t sent_bytes = sendfile(fd, asset_fd, &offset, size);
        if (sent_bytes == -1) {
            perror("[ERR] send_asset sendfile"); free(asset_fname); return -1;
        }
        size -= sent_bytes;
    }
    free(asset_fname);
    return 0;
}

int receive_asset(int fd, struct sockaddr_in addr, char *aid, char *asset_fname, long size) {
    char asset_path[SIZE_ASSET_FILE_PATH+1];
    sprintf(asset_path, "%s/%s/ASSET/%s", PATH_AUCTIONS, aid, asset_fname);
    char *data = malloc(size);
    if (data == NULL) {
        perror("[ERR] receive_asset malloc"); return -1;
    }
    FILE *file = fopen(asset_path, "wb");
    if (file == NULL) {
        perror("[ERR] receive_asset fopen"); free(data); return -1;
    }
    ssize_t n = 0;
    do { // read bytes of file and write
        n = recv(fd, data, size, 0);
        if(n==-1) { 
            perror("[ERR] receive_asset recv"); 
            fclose(file); 
            free(data); 
            return -1;
        }
        size_t bytes_written = fwrite(data, 1, n, file);
        if (bytes_written != n) {
            perror("[ERR] receive_asset fwrite"); 
            fclose(file); 
            free(data); 
            return -1;
        }
        size -= n;
    } while (size != 0);
    fclose(file);
    free(data);
    return 0;
}

bool is_registered(char *uid) {
    char pass_path[SIZE_PASS_FILE_PATH+1];
    sprintf(pass_path, "%s/%s/%s_pass.txt", PATH_USERS, uid, uid);
    return file_exists(pass_path);
}

bool is_logged_in(char *uid) {
    char login_path[SIZE_LOGIN_FILE_PATH+1];
    sprintf(login_path, "%s/%s/%s_login.txt", PATH_USERS, uid, uid);
    return file_exists(login_path);
}

bool is_auction_owned(char *uid, char *aid) {
    char hosted_auction_fname[SIZE_HOSTED_FILE_PATH+1];
    sprintf(hosted_auction_fname, "%s/%s/HOSTED/%s.txt", PATH_USERS, uid, aid);
    return file_exists(hosted_auction_fname);
}

bool is_auction_active(char *aid) {
    long fulltime, timeactive;
    char file_name[100];

    if (!end_file_exists(aid)) {
        sprintf(file_name, "%s/%s/START_%s.txt", PATH_AUCTIONS, aid, aid);
        FILE *start_file = fopen(file_name, "r");
        if (start_file == NULL) {
            perror("[ERR] is_auction_active fopen"); return false;
        }
        
        fscanf(start_file, "%*s %*s %*s %*s %ld %*s %*s %ld", &timeactive, &fulltime);
        fclose(start_file);
        time_t t;
        if (time(&t) - fulltime > timeactive) {
            close_auction(aid, fulltime, fulltime+timeactive);       
            return false;
        }
        return true;
    }
    return false;
}

bool is_correct_password(char *password, char *uid) {
    char correct_pass[SIZE_PASSWORD+1];
    char pass_fname[100];

    sprintf(pass_fname, "%s/%s/%s_pass.txt", PATH_USERS, uid, uid);

    FILE *pass_file = fopen(pass_fname, "r");
    if (pass_file != NULL) {
        fgets(correct_pass, sizeof(correct_pass), pass_file);
        if (strcmp(correct_pass, password) == 0) {
            fclose(pass_file);
            return true;
        } 
    } else {
        perror("[ERR] is_correct_password fopen"); return false;
    }
    fclose(pass_file);
    return false;
}

bool is_bid_too_small(char *aid, int bid_value) {
    char bids_path[SIZE_BIDS_PATH+1];
    DIR *bids_dir;
    struct dirent *entry;
    char val[7];

    sprintf(bids_path, "%s/%s/BIDS", PATH_AUCTIONS, aid);

    bids_dir = opendir(bids_path);
    if (bids_dir == NULL) {
        perror("[ERR] is_bid_too_small opendir"); return false;
    }
    // Iterate through each entry in the directory
    while ((entry = readdir(bids_dir)) != NULL) {
        // Skip current and parent directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        for (int i=0; i<strlen(entry->d_name); i++) {
            // stop at .txt
            if (entry->d_name[i] == '.') break;
            val[i] = entry->d_name[i+4];
        }
        if (atoi(val) >= bid_value)  {
            closedir(bids_dir);
            return true;
        }    
    }
    closedir(bids_dir);
    return false;
}

bool auction_exists(char *aid) {
    char auction_path[SIZE_AUCTION_PATH+1];
    sprintf(auction_path, "%s/%s", PATH_AUCTIONS, aid);
    return directoryExists(auction_path);
}


bool directoryExists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

bool has_started_auctions(char *uid) {
    char hosted_path[SIZE_HOSTED_PATH+1];
    DIR *hosted_dir;
    struct dirent *entry;

    sprintf(hosted_path, "%s/%s/HOSTED", PATH_USERS, uid);

    hosted_dir = opendir(hosted_path);
    if (hosted_dir == NULL) {
        perror("[ERR] Unable to open directory"); return false;
    }
    while ((entry = readdir(hosted_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        // has files besides current and parent directories
        closedir(hosted_dir);
        return true;
    }
    closedir(hosted_dir);
    return false;
}

bool has_placed_bids(char *uid) {
    char bidded_path[SIZE_BIDDED_PATH+1];
    DIR *bidded_dir;
    struct dirent *entry;

    sprintf(bidded_path, "%s/%s/BIDDED", PATH_USERS, uid);

    bidded_dir = opendir(bidded_path);
    if (bidded_dir == NULL) {
        perror("[ERR] has_placed_bids opendir"); return false;
    }
    while ((entry = readdir(bidded_dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        // has files besides current and parent directories
        closedir(bidded_dir);
        return true;
    }
    closedir(bidded_dir);
    return false;
}

bool end_file_exists(char *aid) {
    char end_file_path[SIZE_END_FILE_PATH+1];
    sprintf(end_file_path, "%s/%s/END_%s.txt", PATH_AUCTIONS, aid, aid);
    return file_exists(end_file_path);
}

bool file_exists(const char *filename) {
    return access(filename, F_OK) != -1;
}

void try_to_login(int fd, struct sockaddr_in addr, char *uid, char *password) {
    if (is_correct_password(password, uid)) {
        if (create_login_file(uid) == 0) send_reply_to_user(fd, addr, "RLI OK\n");
    } else {
        send_reply_to_user(fd, addr, "RLI NOK\n");
    }
}

int logout(char *login_file_name, int fd, struct sockaddr_in addr, char *uid) {
    sprintf(login_file_name, "%s/%s/%s_login.txt", PATH_USERS, uid, uid);
    if (remove(login_file_name) != 0) {
        perror("[ERR] logout remove"); return -1;
    }
    return 0;
}

int unregister(char *login_file_name, char *pass_file_name, int fd, struct sockaddr_in addr, char *uid) {
    sprintf(login_file_name, "%s/%s/%s_login.txt", PATH_USERS, uid, uid);
    sprintf(pass_file_name, "%s/%s/%s_pass.txt", PATH_USERS, uid, uid);

    if (remove(login_file_name) != 0 || remove(pass_file_name) != 0) {
        perror("[ERR] unregister remove"); return -1;
    }
    return 0;
}

int register_user(int fd, struct sockaddr_in addr, char *uid, char *password) {
    char path[SIZE_HOSTED_PATH+1];
    sprintf(path, "%s/%s", PATH_USERS, uid);
    if (!directoryExists(path)) {
        if (mkdir(path, 0777) != 0) {
            perror("[ERR] register_user mkdir"); return -1;
        }
    }
    sprintf(path, "%s/%s/HOSTED", PATH_USERS, uid);
    if (!directoryExists(path)) {
        if (mkdir(path, 0777) != 0) {
            perror("[ERR] register_user mkdir"); return -1;
        }
    }
    sprintf(path, "%s/%s/BIDDED", PATH_USERS, uid);
    if (!directoryExists(path)) {
        if (mkdir(path, 0777) != 0) {
            perror("[ERR] register_user mkdir"); return -1;
        }
    }
    if (create_login_file(uid) == -1 || create_pass_file(uid, password) == -1) 
        return -1;
    send_reply_to_user(fd, addr, "RLI REG\n");
    return 0;
}

int register_auction(int fd, struct sockaddr_in addr, char *uid, char *name, char *asset,
 char *start_value, char *timeactive, char *aid) {
    char path[SIZE_ASSET_FILE_PATH+1]; // biggest path

    sprintf(path, "%s/%s", PATH_AUCTIONS, aid);
    if (!directoryExists(path)) {
        if (mkdir(path, 0777) != 0) {
            perror("[ERR] register_auction mkdir AID"); return -1;
        }
    }
    if (start_auction(uid, name, asset, start_value, timeactive, aid) == -1)
        return -1;

    sprintf(path, "%s/%s/ASSET", PATH_AUCTIONS, aid);
    if (mkdir(path, 0777) != 0) {
        perror("[ERR] register_auction mkdir ASSET"); return -1;
    }
    sprintf(path, "%s/%s/BIDS", PATH_AUCTIONS, aid);
    if (mkdir(path, 0777) != 0) {
        perror("[ERR] register_auction mkdir BIDS"); return -1;
    }
    sprintf(path, "%s/%s/HOSTED/%s.txt", PATH_USERS, uid, aid);
    FILE *host_file = fopen(path, "w");
    if (host_file == NULL) {
        perror("[ERR] register_auction fopen"); return -1;
    }
    fclose(host_file);
    return 0;
}

int start_auction(char *uid, char *name, char *asset,
 char *start_value, char *timeactive, char *aid) 
{
    char start_file_path[SIZE_START_FILE_PATH+1];
    time_t t = time(NULL);
    struct tm *start_time = gmtime(&t);
    sprintf(start_file_path, "%s/%s/START_%s.txt", PATH_AUCTIONS, aid, aid);
    FILE *start_file = fopen(start_file_path, "w");

    if (start_file != NULL) {
        fprintf(start_file, "%s %s %s %s %s %4d-%02d-%02d %02d:%02d:%02d %ld", 
        uid, name, asset, start_value, timeactive, 
        start_time->tm_year+1900, start_time->tm_mon+1, start_time->tm_mday,
        start_time->tm_hour, start_time->tm_min, start_time->tm_sec, t);
        fclose(start_file);
        return 0;
    } else {
        perror("[ERR] start_auction fopen"); return -1;
    }
}

int close_auction(char *aid, long start_fulltime, long end_fulltime) {
    char end_file_path[SIZE_END_FILE_PATH+1];
    struct tm *end_time = gmtime(&end_fulltime); 

    if (start_fulltime == 0) {
        start_fulltime = get_start_fulltime(aid);
        if (start_fulltime == -1) return -1;
    }
    sprintf(end_file_path, "%s/%s/END_%s.txt", PATH_AUCTIONS, aid, aid);
    FILE *end_file = fopen(end_file_path, "w");
    if (end_file != NULL) {
            fprintf(end_file, "%4d-%02d-%02d %02d:%02d:%02d %ld", 
            end_time->tm_year+1900, end_time->tm_mon+1, end_time->tm_mday,
            end_time->tm_hour, end_time->tm_min, end_time->tm_sec, end_fulltime-start_fulltime);
            fclose(end_file);
            return 0;
    } else {
        perror("[ERR] close_auction fopen END"); return -1;
    }
}

int create_login_file(char *uid) {
    char login_file_path[SIZE_LOGIN_FILE_PATH+1];
    sprintf(login_file_path, "%s/%s/%s_login.txt", PATH_USERS, uid, uid);

    FILE *login_file = fopen(login_file_path, "w");
    if (login_file == NULL) {
        perror("[ERR] create_login_file fopen"); return -1;
    }
    fclose(login_file);
    return 0;
}

int create_pass_file(char *uid, char *password) {
    char pass_file_path[SIZE_PASS_FILE_PATH+1];
    sprintf(pass_file_path, "%s/%s/%s_pass.txt", PATH_USERS, uid, uid);

    FILE *pass_file = fopen(pass_file_path, "w");
    if (pass_file != NULL) {
        fprintf(pass_file, "%s", password);
        fclose(pass_file);
    } else {
        perror("[ERR] create_pass_file fopen"); return -1;
    }
    return 0;
}

int create_bid(char *aid, char *uid, int value) {
    if (create_bids_file(aid, uid, value) == -1 || create_bidded_file(aid, uid) == -1) {
        return -1;
    }
    return 0;
}

int create_bids_file(char *aid, char *uid, int value) {
    time_t t = time(NULL);
    struct tm *start_time = gmtime(&t);
    long bid_sec_time = t - get_start_fulltime(aid);

    char bid_file_path[SIZE_BIDS_FILE_PATH+1];
    sprintf(bid_file_path, "%s/%s/BIDS/%06d.txt", PATH_AUCTIONS, aid, value);

    FILE *bids_file = fopen(bid_file_path, "w");
    if (bids_file != NULL) {
        fprintf(bids_file, "%s %d %4d-%02d-%02d %02d:%02d:%02d %ld", 
        uid, value,
        start_time->tm_year+1900, start_time->tm_mon+1, start_time->tm_mday,
        start_time->tm_hour, start_time->tm_min, start_time->tm_sec, bid_sec_time);
        fclose(bids_file);
        return 0;
    } else {
        perror("[ERR] create_bids_file fopen"); return -1;
    } 
}

int create_bidded_file(char *aid, char *uid) {
    char bidded_file_path[SIZE_BIDDED_FILE_PATH+1];
    sprintf(bidded_file_path, "%s/%s/BIDDED/%s.txt", PATH_USERS, uid, aid);

    FILE *bidded_file = fopen(bidded_file_path, "w");
    if (bidded_file == NULL) {
        perror("[ERR] create_bidded_file fopen"); return -1;
    }
    fclose(bidded_file);
    return 0;
}

char* create_list_auctions(char *path_dir) {
    size_t num_auctions = get_num_auctions(path_dir);
    if (num_auctions == -1) return NULL;
    Auction *auctions = get_auctions(path_dir, num_auctions);
    if (auctions == NULL) return NULL;

    size_t len = 0;
    for (size_t i=0; i<num_auctions; i++) {
        len += 6;  // Length of "AID state "
    }
    char *list_auctions = (char*)malloc(len+1); 
    if (list_auctions == NULL) {
        perror("[ERR] create_list_auctions malloc"); return NULL;
    }
    size_t currentPos = 0;
    for (size_t i=0; i<num_auctions; i++) {
        sprintf(list_auctions + currentPos, "%s %d ", auctions[i].aid, auctions[i].state);
        currentPos += strlen(auctions[i].aid) + 3; // +3 for 2 spaces + state (0 or 1);
    }
    free(auctions);
    list_auctions[currentPos] = '\0';
    return list_auctions;
}

char *create_list_bids(char *aid) {
    size_t num_bids = get_num_bids(aid);
    if (num_bids == -1) return NULL;
    Bid *bids = get_bids(aid, num_bids);
    if (bids == NULL) return NULL;

    size_t len = 0;
    for (size_t i=0; i<num_bids; i++) {
        len += 200;  
    }
    char *list_bids = (char*)malloc(len+1); 
    if (list_bids == NULL) {
        perror("[ERR] Memory allocation error"); free(bids); return NULL;
    }
    size_t currentPos = 0;
    for (size_t i=0; i<num_bids; i++) {
        sprintf(list_bids + currentPos, "B %s %d %s %s %d ",
        bids[i].uid, bids[i].value, bids[i].date, bids[i].time, bids[i].sec_time);

        currentPos += strlen(bids[i].uid) + get_num_digits(bids[i].value) + strlen(bids[i].date)
        + strlen(bids[i].time) + get_num_digits(bids[i].sec_time) + 7; // + 7 for B and spaces
    }
    free(bids);
    list_bids[currentPos-1] = '\0';
    return list_bids;
}

char *create_record(char *aid) {
    char *auction_info = get_auction_info(aid);
    if (auction_info == NULL) return NULL;

    char *record = malloc(strlen(auction_info) + 1); 
    if (record == NULL) {
        perror("[ERR] create_record malloc"); free(auction_info);
        return NULL;
    }
    strcpy(record, auction_info);

    char *list_bids = create_list_bids(aid);
    if (list_bids == NULL) {
        free(auction_info); free(record);
        return NULL;
    }
    if (list_bids[0] != '\0') {
        record = realloc(record, strlen(auction_info) + strlen(list_bids) + 2);
        if (record == NULL) {
            perror("[ERR] create_record realloc list_bids");
            free(auction_info); free(list_bids);
            return NULL;
        }
        strcat(record, " ");
        strcat(record, list_bids);
    }
    if (end_file_exists(aid)) {
        char *end_info = get_end_info(aid);
        if (end_info == NULL) {
            free(record); free(auction_info); free(list_bids);
            return NULL;
        }
        record = realloc(record, strlen(auction_info) + strlen(list_bids)
        + strlen(end_info) + 2);
        if (record == NULL) {
            perror("[ERR] create_record realloc end_file");
            free(end_info); free(auction_info); free(list_bids);
            return NULL;
        }
        strcat(record, " ");
        strcat(record, end_info);
        free(end_info);
    }
    free(auction_info);
    free(list_bids);
    return record;
}

size_t get_num_auctions(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("[ERR] Error opening directory"); return -1;
    }
    size_t count = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strnlen(entry->d_name, 3) >= 3) {
            char temp[4]; 
            strncpy(temp, entry->d_name, 3);
            temp[3] = '\0'; 
            // only process files/directories with name aid
            if (is_AID(temp)) count++;
        }
    }
    closedir(dir);
    return count;
}

size_t get_num_bids(const char *aid) {
    char bids_path[SIZE_BIDS_PATH+1];
    sprintf(bids_path, "%s/%s/BIDS", PATH_AUCTIONS, aid);

    DIR *dir = opendir(bids_path);
    if (dir == NULL) {
        perror("[ERR] get_num_bids opendir"); return -1;
    }
    size_t count = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strlen(entry->d_name) == SIZE_BIDS_FNAME) {
            count++;
        }
    }
    closedir(dir);
    return count;
}

char* get_auction_info(char *aid) {
    char start_file_path[SIZE_START_FILE_PATH+1];
    char *auction_info = (char*)malloc(MAX_SIZE_START_FILE+1);
    if (auction_info == NULL) {
        perror("[ERR] get_auction_info malloc"); return NULL;
    }
    sprintf(start_file_path, "%s/%s/START_%s.txt", PATH_AUCTIONS, aid, aid);

    FILE *start_file = fopen(start_file_path, "r");
    if (start_file == NULL) {
        perror("[ERR] get_auction_info fopen"); return NULL;
    }
    fgets(auction_info, MAX_SIZE_START_FILE, start_file);
    filtrate_info(auction_info);

    fclose(start_file);
    return auction_info;
}

char *get_end_info(char *aid) {
    char *file_contents = (char*)malloc(MAX_SIZE_END_FILE+1);
    if (file_contents == NULL) {
        perror("[ERR] get_end_info file_contents malloc"); return NULL;
    }
    char end_file_path[SIZE_END_FILE_PATH+1];
    sprintf(end_file_path, "%s/%s/END_%s.txt", PATH_AUCTIONS, aid, aid);

    FILE *end_file = fopen(end_file_path, "r");
    if (end_file == NULL) {
        perror("[ERR] get_end_info fopen"); free(file_contents); return NULL;
    } else {
        fgets(file_contents, MAX_SIZE_END_FILE, end_file);
        char *end_info = (char*)malloc(strlen(file_contents)+3);
        if (end_info == NULL) {
            perror("[ERR] get_end_info end_info malloc"); 
            free(file_contents); fclose(end_file); return NULL;
        }
        sprintf(end_info, "E %s", file_contents);
        free(file_contents);
        fclose(end_file);
        return end_info;
    }
}

Auction* get_auctions(const char *path_dir, size_t num_auctions) {
    Auction *auctions = (Auction*)malloc(num_auctions * sizeof(Auction));
    if (auctions == NULL) {
        perror("[ERR] get_auctions malloc"); return NULL;
    }
    DIR *dir = opendir(path_dir);
    if (dir == NULL) {
        perror("[ERR] get_auctions opendir"); return NULL;
    }
    size_t index = 0;
    struct dirent *entry;

    // Extract auctions
    while ((entry = readdir(dir)) != NULL) {
        if (strnlen(entry->d_name, 3) >= 3) {
            char temp[4]; 
            strncpy(temp, entry->d_name, 3);
            temp[3] = '\0'; 

            // only process files/directories with name aid
            if (is_AID(temp)) {
                strncpy(auctions[index].aid, temp, 3);
                auctions[index].aid[3] = '\0';

                if (is_auction_active(auctions[index].aid)) {
                    auctions[index].state = 1;
                } else {
                    auctions[index].state = 0;
                }
                index++;
            }
        }
    }
    closedir(dir);
    qsort(auctions, num_auctions, sizeof(Auction), compare_auctions);
    return auctions;
}

Bid* get_bids(char *aid, size_t num_bids) {
    Bid *bids = (Bid*)malloc(num_bids*sizeof(Bid));
    if (bids == NULL) {
        perror("[ERR] get_bids malloc"); return NULL;
    }
    char path_bids[100];
    sprintf(path_bids, "%s/%s/BIDS", PATH_AUCTIONS, aid);

    DIR *dir = opendir(path_bids);
    if (dir == NULL) {
        perror("[ERR] get_bids opendir"); free(bids); return NULL;
    }
    size_t index = 0;
    struct dirent *entry;
    // Extract bids
    while ((entry = readdir(dir)) != NULL) {
        // only process bid files
        if (entry->d_type == DT_REG && strlen(entry->d_name) == 10) {
            if (add_bid_to_list(bids, aid, entry->d_name, index) == -1) {
                return NULL;
            }
            index++;
        }
    }
    closedir(dir);
    qsort(bids, num_bids, sizeof(Bid), compare_bids);
    return bids;
}

long get_file_size(char *fname) {
    struct stat filestat;
       
    if (stat(fname, &filestat) == 0) {
        return filestat.st_size;
    } else {
        perror("[ERR] get_file_size"); return -1;
    }
}

int get_num_digits(int value) {
    int numDigits = 0;

    if (value == 0) {
        return 1;
    }
    while (value != 0) {
        value /= 10;
        numDigits++;
    }
    return numDigits;
}

char* get_aid() {
    DIR *auctions_dir;
    struct dirent *entry;
    int max_value = 0;

    char *aid = (char*)malloc(SIZE_AID+1);
    if (aid == NULL) {
        perror("[ERR] get_aid malloc"); return NULL;
    }
    auctions_dir = opendir(PATH_AUCTIONS);
    if (auctions_dir == NULL) {
        perror("[ERR] get_aid opendir");
        return "";
    }
    while ((entry = readdir(auctions_dir)) != NULL) {
        if (entry->d_type == DT_DIR && strlen(entry->d_name) == 3) {
            int current_value = atoi(entry->d_name);
            if (current_value > max_value) {
                max_value = current_value;
            }
        }
    }
    closedir(auctions_dir);
    if (max_value < MAX_NUM_AID) {
        max_value++; snprintf(aid, SIZE_AID+1, "%03d", max_value);
    }
    else {
        snprintf(aid, SIZE_AID+1, "%03d", 0);
    }
    return aid;
}

long get_start_fulltime(char *aid) {
    long start_fulltime = 0;
    char start_file_path[SIZE_START_FILE_PATH+1];
    sprintf(start_file_path, "%s/%s/START_%s.txt", PATH_AUCTIONS, aid, aid);

    FILE *start_file = fopen(start_file_path, "r");
    if (start_file == NULL) {
        perror("[ERR] get_start_fulltime fopen"); return -1;
    }
    if (fscanf(start_file,"%*s %*s %*s %*s %*s %*s %*s %ld", &start_fulltime) != 1) {
        perror("[ERR] get_start_fulltime fscanf"); return -1;
    }
    fclose(start_file);
    return start_fulltime;
}

char* get_asset_name(char *aid) {
    char start_file_path[SIZE_START_FILE_PATH+1];
    char *asset_fname = malloc(MAX_FILENAME);
    if (asset_fname == NULL) {
        perror("[ERR] get_asset_name malloc"); return NULL;
    }
    sprintf(start_file_path, "%s/%s/START_%s.txt", PATH_AUCTIONS, aid, aid);

    FILE *start_file = fopen(start_file_path, "r");
    if (start_file != NULL) {
        if (fscanf(start_file, "%*s %*s %s", asset_fname) != 1) {
            perror("[ERR] get_asset_name fscanf"); return NULL;
        }
        fclose(start_file);
    } else {
        perror("[ERR] get_asset_name fopen"); return NULL;
    }
    return asset_fname;
}

int compare_auctions(const void *a, const void *b) {
    const Auction *auction1 = (const Auction*)a;
    const Auction *auction2 = (const Auction*)b;
    return strcmp(auction1->aid, auction2->aid);
}

int compare_bids(const void *a, const void *b) {
    const Bid *bid1 = (const Bid*)a;
    const Bid *bid2 = (const Bid*)b;
    return bid1->value > bid2->value;
}

int add_bid_to_list(Bid *bids, char *aid, char *bid_fname, int index) {
    char bid_info[100];
    char path_bid[SIZE_BIDS_FILE_PATH+1];
    sprintf(path_bid, "%s/%s/BIDS/%s", PATH_AUCTIONS, aid, bid_fname);

    FILE *bid_file = fopen(path_bid, "r");
    if (bid_file == NULL) {
        perror("[ERR] add_bid_to_list fopen"); return -1;
    }
    fgets(bid_info, sizeof(bid_info), bid_file);
    if (sscanf(bid_info, "%s %d %s %s %d", bids[index].uid, &bids[index].value, bids[index].date,
    bids[index].time, &bids[index].sec_time) != 5) {
        perror("[ERR] add_bid_to_list sscanf"); return -1;
    }
    fclose(bid_file);
    return 0;
}

void filtrate_info(char *auction_info) {
    char uid[SIZE_UID+1];
    char auction_name[100];
    char asset_fname[MAX_FILENAME+1];
    int start_value;
    int time_active;
    char start_date[SIZE_DATE+1];
    char start_time[SIZE_TIME+1];

    sscanf(auction_info, "%s %s %s %d %d %s %s", uid, auction_name, asset_fname,
    &start_value, &time_active, start_date, start_time);

    sprintf(auction_info, "%s %s %s %d %s %s %d", uid, auction_name, asset_fname,
    start_value, start_date, start_time, time_active);
}







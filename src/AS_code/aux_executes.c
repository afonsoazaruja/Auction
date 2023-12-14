#include "aux_executes.h"
#include <dirent.h>
#include <stdio.h>

void send_reply_to_user(int fd, struct sockaddr_in addr, char *reply) {
    if (sendto(fd, reply, strlen(reply), 0,
    (struct sockaddr*)&addr, sizeof(addr)) == -1) exit(1);
}

bool is_registered(char *uid) {
    char pass_fname[100];
    sprintf(pass_fname, "%s/%s/%s_pass.txt", PATH_USERS_DIR, uid, uid);

    FILE *pass_file = fopen(pass_fname, "r");
    if (pass_file == NULL) return false;
    
    fclose(pass_file);
    return true;
}

bool is_logged_in(char *uid) {
    char login_fname[100];
    sprintf(login_fname, "%s/%s/%s_login.txt", PATH_USERS_DIR, uid, uid);

    FILE *login_file = fopen(login_fname, "r");
    if (login_file == NULL) return false;
    
    fclose(login_file);
    return true;
}

bool is_auction_active(char *aid) {
    long fulltime, timeactive;
    char file_name[100];

    sprintf(file_name, "%s/%s/END_%s.txt", PATH_AUCTIONS_DIR, aid, aid);
    FILE *end_file = fopen(file_name, "r");

    if (end_file == NULL) {
        sprintf(file_name, "%s/%s/START_%s.txt", PATH_AUCTIONS_DIR, aid, aid);
        FILE *start_file = fopen(file_name, "r");
        fscanf(start_file, "%*s %*s %*s %*s %ld %*s %*s %ld", &timeactive, &fulltime);
        time_t t;
        fclose(start_file);
        if (time(&t) - fulltime > timeactive) {            
            close_auction(aid);
            return false;
        }
        return true;
    }
    fclose(end_file);
    return false;
}

bool is_auction_owned(char *uid, char *aid) {
    char hosted_auction_fname[100];
    sprintf(hosted_auction_fname, "%s/%s/HOSTED/%s.txt", PATH_USERS_DIR, uid, aid);

    FILE *aid_file = fopen(hosted_auction_fname, "r");
    if (aid_file == NULL) return false;
      
    fclose(aid_file);
    return true;
}

bool is_correct_password(char *password, char *uid) {
    char correct_pass[SIZE_PASSWORD+1];
    char pass_fname[100];

    sprintf(pass_fname, "%s/%s/%s_pass.txt", PATH_USERS_DIR, uid, uid);
    FILE *pass_file = fopen(pass_fname, "r");

    if (pass_file != NULL) {
        fgets(correct_pass, sizeof(correct_pass), pass_file);
        if (strcmp(correct_pass, password) == 0) {
            fclose(pass_file);
            return true;
        } 
    } else {
        perror("Error opening file"); exit(1);
    }
    fclose(pass_file);
    return false;
}

bool is_auct_hosted_by_user(char *aid, char *uid) {
    char auction_file_name[32];
    sprintf(auction_file_name, "%s/%s/HOSTED/%s.txt", PATH_USERS_DIR, uid, aid);
    
    if (fopen(auction_file_name, "r") == NULL) return false;
    return true;
}

bool is_bid_too_small(char *aid, int bid_value) {
    char bids_path[100];
    DIR *bids_dir;
    struct dirent *entry;
    char val[7];

    sprintf(bids_path, "%s/%s/BIDS", PATH_AUCTIONS_DIR, aid);

    bids_dir = opendir(bids_path);
    if (bids_dir == NULL) {
        perror("Unable to open directory"); exit(1);
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
        if (atoi(val) > bid_value)  {
            closedir(bids_dir);
            return true;
        }    
    }
    closedir(bids_dir);
    return false;
}

bool directoryExists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

bool has_started_auctions(char *uid) {
    char hosted_path[100];
    DIR *hosted_dir;
    struct dirent *entry;

    sprintf(hosted_path, "%s/%s/HOSTED", PATH_USERS_DIR, uid);

    hosted_dir = opendir(hosted_path);
    if (hosted_dir == NULL) {
        perror("Unable to open directory"); exit(1);
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
    char bidded_path[100];
    DIR *bidded_dir;
    struct dirent *entry;

    sprintf(bidded_path, "%s/%s/BIDDED", PATH_USERS_DIR, uid);

    bidded_dir = opendir(bidded_path);
    if (bidded_dir == NULL) {
        perror("Unable to open directory"); exit(1);
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

void try_to_login(int fd, struct sockaddr_in addr, char *uid, char *password) {
    if (is_correct_password(password, uid)) {
        create_login_file(uid);
        send_reply_to_user(fd, addr, "RLI OK\n");
    } else {
        send_reply_to_user(fd, addr, "RLI NOK\n");
    }
}

void register_user(int fd, struct sockaddr_in addr, char *uid, char *password) {
    char path[100];
    sprintf(path, "%s/%s", PATH_USERS_DIR, uid);
    if (!directoryExists(path)) {
        if (mkdir(path, 0777) != 0) {
            perror("Error creating directory"); exit(1);
        }
    }
    sprintf(path, "%s/%s/HOSTED", PATH_USERS_DIR, uid);
    if (!directoryExists(path)) {
        if (mkdir(path, 0777) != 0) {
            perror("Error creating directory"); exit(1);
        }
    }
    sprintf(path, "%s/%s/BIDDED", PATH_USERS_DIR, uid);
    if (!directoryExists(path)) {
        if (mkdir(path, 0777) != 0) {
            perror("Error creating directory"); exit(1);
        }
    }
    create_login_file(uid);
    create_pass_file(uid, password);
    send_reply_to_user(fd, addr, "RLI REG\n");
}

void register_auction(int fd, struct sockaddr_in addr, char *uid, char *name, char *asset, char *start_value, char *timeactive, char *aid) {
    char path[100];

    sprintf(path, "%s/%s", PATH_AUCTIONS_DIR, aid);
    if (!directoryExists(path)) {
        if (mkdir(path, 0777) != 0) {
            perror("Error creating directory auction"); exit(1);
        }
    }
    create_start_auction_file(uid, name, asset, start_value, timeactive, aid);

    sprintf(path, "%s/%s/ASSET", PATH_AUCTIONS_DIR, aid);
    if (mkdir(path, 0777) != 0) {
        perror("Error creating directory ASSET"); exit(1);
    }
    sprintf(path, "%s/%s/BIDS", PATH_AUCTIONS_DIR, aid);
    if (mkdir(path, 0777) != 0) {
        perror("Error creating directory BIDS"); exit(1);
    }
    sprintf(path, "%s/%s/HOSTED/%s.txt", PATH_USERS_DIR, uid, aid);
    FILE *host_file = fopen(path, "w");
    if (host_file == NULL) {
        perror("Error opening file"); exit(1);
    }
    fclose(host_file);
}

void close_auction(char *aid) {
    char end_fname[100];
    sprintf(end_fname, "%s/%s/END_%s.txt", PATH_AUCTIONS_DIR, aid, aid);

    FILE *end_file = fopen(end_fname, "w");
    if (end_file == NULL) {
        perror("Error creating end file"); exit(1);
    }
    fclose(end_file);
}

void create_login_file(char *uid) {
    char login_fname[100];
    sprintf(login_fname, "%s/%s/%s_login.txt", PATH_USERS_DIR, uid, uid);
    FILE *login_file = fopen(login_fname, "w");

    if (login_file == NULL) {
        perror("Error opening file"); exit(1);
    }
    fclose(login_file);
}

void create_pass_file(char *uid, char *password) {
    char pass_fname[100];
    sprintf(pass_fname, "%s/%s/%s_pass.txt", PATH_USERS_DIR, uid, uid);

    FILE *pass_file = fopen(pass_fname, "w");
    if (pass_file != NULL) {
        fprintf(pass_file, "%s", password);
        fclose(pass_file);
    } else {
        perror("Error opening file"); exit(1);
    }
}

void create_start_auction_file(char *uid, char *name, char *asset,
 char *start_value, char *timeactive, char *aid) 
{
    char start_fname[100];
    sprintf(start_fname, "%s/%s/START_%s.txt", PATH_AUCTIONS_DIR, aid, aid);
    FILE *start_file = fopen(start_fname, "w");
    time_t t;
    struct tm *current_time;
    time(&t);
    current_time = gmtime(&t);

    if (start_file != NULL) {
        fprintf(start_file, "%s %s %s %s %s %4d-%02d-%02d %02d:%02d:%02d %ld", uid, name, asset, start_value, timeactive, 
        current_time->tm_year+1900, current_time->tm_mon+1, current_time->tm_mday,
        current_time->tm_hour, current_time->tm_min, current_time->tm_sec, time(&t));
        fclose(start_file);
    } else {
        perror("Error opening file"); exit(1);
    }
}

void create_bid(char *aid, char *uid, int value) {
    char bids_fname[100];
    char bidded_fname[100];

    sprintf(bids_fname, "%s/%s/BIDS/%s_%d.txt", PATH_AUCTIONS_DIR, aid, aid, value);
    sprintf(bidded_fname, "%s/%s/BIDDED/%s.txt", PATH_USERS_DIR, uid, aid);

    FILE *bidded_file = fopen(bidded_fname, "w");
    if (bidded_file == NULL) {
        perror("Error creating file in BIDDED"); exit(1);
    }
    fclose(bidded_file);

    FILE *bids_file = fopen(bids_fname, "w");
    if (bids_file == NULL) {
        perror("Error creating file in BIDS"); exit(1);
    } 
    fclose(bids_file);
}

char* create_list_auctions(char *path_dir) {
    size_t num_bids = get_num_files_in_dir(path_dir);
    Auction *auctions = get_auctions(path_dir, num_bids);

    size_t len = 0;
    for (size_t i=0; i<num_bids; i++) {
        len += 6;  // Length of "AID state "
    }
    char *list_auctions = (char*)malloc(len+1); 
    if (list_auctions == NULL) {
        perror("Memory allocation error"); exit(1);
    }
    size_t currentPos = 0;
    for (size_t i=0; i<num_bids; i++) {
        strcpy(list_auctions + currentPos, auctions[i].aid);
        currentPos += strlen(auctions[i].aid);

        sprintf(list_auctions + currentPos, " %d ", auctions[i].state);
        currentPos += 3; // 2 spaces + state (0 or 1);
    }
    list_auctions[currentPos] = '\0';
    return list_auctions;
}


char* get_aid() {
    char *aid = (char*)malloc(SIZE_AID+1);
    DIR *auctions_dir;
    struct dirent *entry;
    int max_value = 0;

    auctions_dir = opendir(PATH_AUCTIONS_DIR);
    if (auctions_dir == NULL) {
        perror("opendir");
        exit(1);
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
        return aid;
    }
    else {
        snprintf(aid, SIZE_AID+1, "%03d", 0);
        return aid;
    }
}

size_t get_num_files_in_dir(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory with bids"); exit(1);
    }

    size_t count = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            count++;
        }
    }
    closedir(dir);
    return count;
}

Auction* get_auctions(const char *path_dir, size_t num_auctions) {
    Auction *auctions = (Auction*)malloc(num_auctions * sizeof(Auction));
    if (auctions == NULL) {
        perror("Memory allocation error"); exit(1);
    }

    DIR *dir = opendir(path_dir);
    if (dir == NULL) {
        perror("Error opening directory with bids"); exit(1);
    }
    struct dirent *entry;
    size_t index = 0;
    // Extract bids
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            strncpy(auctions[index].aid, entry->d_name, 3);
            auctions[index].aid[3] = '\0';

            if (is_auction_active(auctions[index].aid)) {
                auctions[index].state = 1;
            } else {
                auctions[index].state = 0;
            }
            index++;
        }
    }
    closedir(dir);
    return auctions;
}

long get_file_size(char *fname) {
    struct stat filestat;
       
    if (stat(fname, &filestat) == 0) {
        return filestat.st_size;
    } else {
        perror("Error getting file information");
    }
    return 0; // only to compile
}





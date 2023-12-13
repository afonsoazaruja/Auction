#include "aux_executes.h"
#include "../validations.h"

void send_reply_to_user(int fd, struct sockaddr_in addr, char *reply) {
    if (sendto(fd, reply, strlen(reply), 0,
    (struct sockaddr*)&addr, sizeof(addr)) == -1) exit(1);
}

void build_path_user_dir(char *path_user_dir, char *uid) {
    strcpy(path_user_dir, "../../ASDIR/USERS/");
    strcat(path_user_dir, uid);
}

void build_path_auction_dir(char *path_auction_dir, char *aid) {
    strcpy(path_auction_dir, "../../ASDIR/AUCTIONS/");
    strcat(path_auction_dir, aid);
}

void build_auction_asset_dir(char *path_auction_dir, char *aid) {
    strcpy(path_auction_dir, "../../ASDIR/AUCTIONS/");
    strcat(path_auction_dir, aid);
    strcat(path_auction_dir, "/ASSET");
    
}

void build_auction_bids_dir(char *path_auction_dir, char *aid) {
    strcpy(path_auction_dir, "../../ASDIR/AUCTIONS/");
    strcat(path_auction_dir, aid);
    strcat(path_auction_dir, "/BIDS");
}

bool is_registered(char *path_user_dir, char *uid) {
    char *pass_file_name = get_file_name(path_user_dir, uid, "_pass", ".txt");
    FILE *pass_file = fopen(pass_file_name, "r");
    if (pass_file == NULL) return false;

    fclose(pass_file);
    free(pass_file_name);
    return true;
}

bool is_logged_in(char *path_user_dir, char *uid) {
    char *login_file_name = get_file_name(path_user_dir, uid, "_login", ".txt");
    FILE *login_file = fopen(login_file_name, "r");

    free(login_file_name);
    if (login_file == NULL) return false;

    fclose(login_file);
    return true;
}

bool is_auction_active(char *path_auction_dir, char *aid) {
    char *end_file_name = get_file_name(path_auction_dir, "END_", aid, ".txt");
    FILE *end_file = fopen(end_file_name, "r");
    
    free(end_file_name);
    if (end_file == NULL) return true;

    fclose(end_file);
    return true;
}

void try_to_login(int fd, struct sockaddr_in addr, char *path_user_dir, char *uid, char *password) {
    if (is_correct_password(password, path_user_dir, uid)) {
        create_login_file(uid, path_user_dir);
        send_reply_to_user(fd, addr, "RLI OK\n");
    } else {
        send_reply_to_user(fd, addr, "RLI NOK\n");
    }
}

void register_user(int fd, struct sockaddr_in addr, char *path_user_dir, char *uid, char *password) {
    if (!directoryExists(path_user_dir)) {
        if (mkdir(path_user_dir, 0777) != 0) {
            perror("Error creating directory"); exit(1);
        }
    }
    create_login_file(uid, path_user_dir);
    create_pass_file(uid, password, path_user_dir);
    send_reply_to_user(fd, addr, "RLI REG\n");
}

void register_auction(int fd, struct sockaddr_in addr, char *uid, char *name, char *asset, char *start_value, char *timeactive, char *aid) {
    char auction_dir[100];
    char asset_dir[100];
    char bids_dir[100];

    build_path_auction_dir(auction_dir, aid);
    if (!directoryExists(auction_dir)) {
        if (mkdir(auction_dir, 0777) != 0) {
            perror("Error creating directory auction"); exit(1);
        }
    }
    create_start_auction_file(auction_dir, uid, name, asset, start_value, timeactive, aid);

    build_auction_asset_dir(asset_dir, aid);
    if (mkdir(asset_dir, 0777) != 0) {
        perror("Error creating directory auction"); exit(1);
    }
    build_auction_bids_dir(bids_dir, aid);
    if (mkdir(bids_dir, 0777) != 0) {
        perror("Error creating directory auction"); exit(1);
    }
}

char *get_file_name(char *path_user_dir, char *first, char *second, char *third) {  
    size_t newSize = strlen(path_user_dir) + strlen(first) +
    strlen(second) + strlen(third) + 2; // +2 for / and \0

    char *name = (char *)malloc(newSize);
    if (name != NULL) {
        strcpy(name, path_user_dir);
        strcat(name, "/");
        strcat(name, first);
        strcat(name, second);
        strcat(name, third);
    } else {
        perror("Memory allocation failed"); exit(1);
    }
    return name;
}

bool directoryExists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

void create_pass_file(char *uid, char *password, char *path_user_dir) {
    char *pass_file_name = get_file_name(path_user_dir, uid, "_pass", ".txt");
    FILE *pass_file = fopen(pass_file_name, "w");

    if (pass_file != NULL) {
        free(pass_file_name);
        fprintf(pass_file, "%s", password);
        fclose(pass_file);
    } else {
        free(pass_file_name);
        perror("Error opening file"); exit(1);
    }
}

void create_login_file(char *uid, char *path_user_dir) {
    char *login_file_name = get_file_name(path_user_dir, uid, "_login", ".txt");
    FILE *login_file = fopen(login_file_name, "w");

    if (login_file != NULL) {
        free(login_file_name);
        fclose(login_file);
    } else {
        free(login_file_name);
        perror("Error opening file"); exit(1);
    }
}

void create_start_auction_file(char *auction_dir, char *uid, char *name, char *asset,
char *start_value, char *timeactive, char *aid) {
    char *start_auction = get_file_name(auction_dir, "START_", aid, ".txt");
    FILE *start_file = fopen(start_auction, "w");
    time_t t;
    struct tm *current_time;
    time(&t);
    current_time = gmtime(&t);

    if (start_file != NULL) {
        free(start_auction);
        fprintf(start_file, "%s %s %s %s %s %4d-%02d-%02d %02d:%02d:%02d %ld", uid, name, asset, start_value, timeactive, 
        current_time->tm_year+1900, current_time->tm_mon+1, current_time->tm_mday,
        current_time->tm_hour, current_time->tm_min, current_time->tm_sec, time(&t));
        fclose(start_file);
    } else {
        free(start_auction);
        perror("Error opening file"); exit(1);
    }
}

bool is_correct_password(char *password, char *path_user_dir, char *uid) {
    char *pass_file_name = get_file_name(path_user_dir, uid, "_pass", ".txt");
    char correct_pass[SIZE_PASSWORD + 1];
    FILE *pass_file = fopen(pass_file_name, "r");
    
    free(pass_file_name);

    if (pass_file != NULL) {
        fgets(correct_pass, sizeof(correct_pass), pass_file);
        if (strcmp(correct_pass, password) == 0) return true;
    } else {
        perror("Error opening file"); exit(1);
    }
    return false;
}

void format_aid(char *aid) {
    snprintf(aid, 4, "%03d", num_aid);
}

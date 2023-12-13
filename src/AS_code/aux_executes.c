#include "aux_executes.h"
#include "../validations.h"

char dir[100];

void send_reply_to_user(int fd, struct sockaddr_in addr, char *reply) {
    if (sendto(fd, reply, strlen(reply), 0,
    (struct sockaddr*)&addr, sizeof(addr)) == -1) exit(1);
}

bool is_registered(char *uid) {
    sprintf(dir, "%s/%s/%s_pass.txt", USERS_DIR, uid, uid);
    FILE *pass_file = fopen(dir, "r");
    if (pass_file == NULL) return false;
    fclose(pass_file);
    return true;
}

bool is_logged_in(char *uid) {
    sprintf(dir, "%s/%s/%s_login.txt", USERS_DIR, uid, uid);
    FILE *login_file = fopen(dir, "r");
    if (login_file == NULL) return false;
    fclose(login_file);
    return true;
}

bool is_auction_active(char *aid) {
    sprintf(dir, "%s/%s/END_%s.txt", AUCTIONS_DIR, aid, aid);
    FILE *end_file = fopen(dir, "r");
    if (end_file == NULL) return true;
    fclose(end_file);
    return false;
}

bool is_auction_owned(char *uid, char *aid) {
    sprintf(dir, "%s/%s/HOSTED/%s.txt", USERS_DIR, uid, aid);
    FILE *aid_file = fopen(dir, "r");
    if (aid_file == NULL) return false;
    fclose(aid_file);
    return true;
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
    sprintf(dir, "%s/%s", USERS_DIR, uid);
    if (!directoryExists(dir)) {
        if (mkdir(dir, 0777) != 0) {
            perror("Error creating directory"); exit(1);
        }
    }
    sprintf(dir, "%s/%s/HOSTED", USERS_DIR, uid);
    if (!directoryExists(dir)) {
        if (mkdir(dir, 0777) != 0) {
            perror("Error creating directory"); exit(1);
        }
    }
    sprintf(dir, "%s/%s/BIDDED", USERS_DIR, uid);
    if (!directoryExists(dir)) {
        if (mkdir(dir, 0777) != 0) {
            perror("Error creating directory"); exit(1);
        }
    }
    create_login_file(uid);
    create_pass_file(uid, password);
    send_reply_to_user(fd, addr, "RLI REG\n");
}

void register_auction(int fd, struct sockaddr_in addr, char *uid, char *name, char *asset, char *start_value, char *timeactive, char *aid) {

    sprintf(dir, "%s/%s", AUCTIONS_DIR, aid);
    if (!directoryExists(dir)) {
        if (mkdir(dir, 0777) != 0) {
            perror("Error creating directory auction"); exit(1);
        }
    }
    create_start_auction_file(uid, name, asset, start_value, timeactive, aid);

    sprintf(dir, "%s/%s/ASSET", AUCTIONS_DIR, aid);
    if (mkdir(dir, 0777) != 0) {
        perror("Error creating directory ASSET"); exit(1);
    }
    sprintf(dir, "%s/%s/BIDS", AUCTIONS_DIR, aid);
    if (mkdir(dir, 0777) != 0) {
        perror("Error creating directory BIDS"); exit(1);
    }
    sprintf(dir, "%s/%s/HOSTED/%s.txt", USERS_DIR, uid, aid);
    FILE *host_file = fopen(dir, "w");
    if (host_file != NULL) {
        fclose(host_file);
    } else {
        fclose(host_file);
        perror("Error opening file"); exit(1);
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

char *get_aid() {
    char *aid = (char *)malloc(SIZE_AID+1);
    DIR *dir;
    struct dirent *entry;
    int max_value = 0;

    dir = opendir(AUCTIONS_DIR);
    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && strlen(entry->d_name) == 3) {
            int current_value = atoi(entry->d_name);
            if (current_value > max_value) {
                max_value = current_value;
            }
        }
    }
    closedir(dir);
    if (max_value < MAX_NUM_AID) {
        max_value++; snprintf(aid, SIZE_AID+1, "%03d", max_value);
        return aid;
    }
    else {
        snprintf(aid, SIZE_AID+1, "%03d", 0);
        return aid;
    }
}

bool directoryExists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

void create_pass_file(char *uid, char *password) {
    sprintf(dir, "%s/%s/%s_pass.txt", USERS_DIR, uid, uid);
    FILE *pass_file = fopen(dir, "w");
    if (pass_file != NULL) {
        fprintf(pass_file, "%s", password);
        fclose(pass_file);
    } else {
        perror("Error opening file"); exit(1);
    }
}

void create_login_file(char *uid) {
    sprintf(dir, "%s/%s/%s_login.txt", USERS_DIR, uid, uid);
    FILE *login_file = fopen(dir, "w");

    if (login_file != NULL) {
        fclose(login_file);
    } else {
        perror("Error opening file"); exit(1);
    }
}

void create_start_auction_file(char *uid, char *name, char *asset, char *start_value, char *timeactive, char *aid) {
    sprintf(dir, "%s/%s/START_%s.txt", AUCTIONS_DIR, aid, aid);
    FILE *start_file = fopen(dir, "w");
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

bool is_correct_password(char *password, char *uid) {
    char correct_pass[SIZE_PASSWORD + 1];
    sprintf(dir, "%s/%s/%s_pass.txt", USERS_DIR, uid, uid);
    FILE *pass_file = fopen(dir, "r");

    if (pass_file != NULL) {
        fgets(correct_pass, sizeof(correct_pass), pass_file);
        if (strcmp(correct_pass, password) == 0) return true;
    } else {
        perror("Error opening file"); exit(1);
    }
    return false;
}

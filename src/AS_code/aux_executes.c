#include "aux_executes.h"
#include "../validations.h"
#include <bits/types/cookie_io_functions_t.h>


void send_reply_to_user(int fd, struct sockaddr_in addr, char *reply) {
    int n = sendto(fd, reply, strlen(reply), 0, (struct sockaddr*)&addr, sizeof(addr));
    if (n == -1) exit(1);   
}

void build_path_user_dir(char *path_user_dir, char *uid) {
    strcpy(path_user_dir, "../../ASDIR/USERS/");
    strcat(path_user_dir, uid);
}

bool is_registered(char *path_user_dir, char *uid) {
    char *pass_file_name = get_file_name(uid, path_user_dir, "_pass.txt");
    FILE *pass_file = fopen(pass_file_name, "r");
    if (pass_file == NULL) return false;

    fclose(pass_file);
    free(pass_file_name);
    return true;
}

bool is_logged_in(char *path_user_dir, char *uid) {
    char *login_file_name = get_file_name(uid, path_user_dir, "_login.txt");
    FILE *login_file = fopen(login_file_name, "r");
    if (login_file == NULL) return false;

    fclose(login_file);
    free(login_file_name);
    return true;
}

char *get_file_name(char *uid, char *path_user_dir, char *end_part) {  
    size_t newSize = strlen(path_user_dir) + strlen(uid) +
    strlen(end_part) + 2; // +2 for / and \0

    char *name = (char *)malloc(newSize);
    if (name != NULL) {
        strcpy(name, path_user_dir);
        strcat(name, "/");
        strcat(name, uid);
        strcat(name, end_part);
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
    char *pass_file_name = get_file_name(uid, path_user_dir, "_pass.txt");
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
    char *login_file_name = get_file_name(uid, path_user_dir, "_login.txt");
    FILE *login_file = fopen(login_file_name, "w");

    if (login_file != NULL) {
        free(login_file_name);
        fclose(login_file);
    } else {
        free(login_file_name);
        perror("Error opening file"); exit(1);
    }
}

bool is_correct_password(char *password, char *path_user_dir, char *uid) {
    char *pass_file_name = get_file_name(uid, path_user_dir, "_pass.txt");
    char correct_pass[SIZE_PASSWORD + 1];
    FILE *pass_file = fopen(pass_file_name, "r");

    if (pass_file != NULL) {
        fgets(correct_pass, sizeof(correct_pass), pass_file);
        if (strcmp(correct_pass, password) == 0) return true;
    } else {
        perror("Error opening file"); exit(1);
    }
    free(pass_file_name);
    return false;
}



#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

#define START_DIRECTORY "/workspaces/codespaces-blank"

void permissions(mode_t mode, char result[10]) {
    const mode_t bits[9] = {
        S_IRUSR, S_IWUSR, S_IXUSR,
        S_IRGRP, S_IWGRP, S_IXGRP,
        S_IROTH, S_IWOTH, S_IXOTH
    };
    const char letters[] = "rwxrwxrwx";

    for (int i = 0; i < 9; i++)
        result[i] = (mode & bits[i]) ? letters[i] : '-';
    result[9] = '\0';
}

void search_directory(const char *directory, const char *target) {
    DIR *dir = opendir(directory);
    if (dir == NULL) {
        perror(directory);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        size_t needed = strlen(directory) + strlen(entry->d_name) + 2;
        char *path = malloc(needed);
        if (path == NULL) {
            fprintf(stderr, "Out of memory\n");
            closedir(dir);
            exit(EXIT_FAILURE);
        }
        snprintf(path, needed, "%s/%s", directory, entry->d_name);

        struct stat info;
        if (lstat(path, &info) == -1) {
            perror(path);
            free(path);
            continue;
        }

        if (strstr(entry->d_name, target) != NULL) {
            char mode[10];
            permissions(info.st_mode, mode);
            printf("%s %s\n", path, mode);
        }

        if (S_ISDIR(info.st_mode))
            search_directory(path, target);

        free(path);
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *target = argc == 2 ? argv[1] : "";
    search_directory(START_DIRECTORY, target);
    return EXIT_SUCCESS;
}

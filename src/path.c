#include "constants.h"
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

const char *bin_path;

void init_path() {
    char path[1024];

    ssize_t length = readlink("/proc/self/exe", path, sizeof(path) - 1);

    path[length] = '\0';

    *(strrchr(path, '/') + 1) = '\0';

    bin_path = strdup(path);
}

char *get_path(char *relative_path) {
    int length = strlen(bin_path) + strlen(relative_path) + 1;

    char path[length];

    strcpy(path, bin_path);

    strcat(path, relative_path);

    return strdup(path);
}

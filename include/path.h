#ifndef PATH
#define PATH

extern const char* bin_path;

void init_path();

char *get_path(char *relative_path);

#endif

#ifndef PRACTICE_H
#define PRACTICE_H

#include <stddef.h>

#define MAX_PRACTICES 100
#define MAX_PRACTICE_NAME 256

void get_full_practices_dir(char *full_path, size_t size);
int get_practices(char practices[MAX_PRACTICES][MAX_PRACTICE_NAME], int max_practices);
int practices_initialized(void);
int initialize_practices(void);
int update_practices(void);
int list_practices(void);
int run_practice(const char *practice_name);
int configure_practice(const char *practice_name);

#endif // PRACTICE_H

#ifndef PRACTICE_MANAGER_H
#define PRACTICE_MANAGER_H

#define MAX_PRACTICES 100
#define MAX_PRACTICE_NAME 256

typedef void (*practice_callback)(const char *practice_name, void *user_data);

int practices_initialized(void);
int initialize_practices(void);
int update_practices(void);
int get_practices(char practices[][MAX_PRACTICE_NAME], int max_practices);
int run_practice(const char *practice_name);
int configure_practice(const char *practice_name);

#endif // PRACTICE_MANAGER_H

// core.h

#ifndef CORE_H
#define CORE_H

#include <libintl.h>
#include <locale.h>

#define _(String) gettext(String)
#define MAX_PRACTICE_NAME 256
#define MAX_PHASES 10
#define MAX_STEPS 20
#define MAX_PRACTICES 100
#define MAX_SOUNDS 5
#define MAX_STRING_LENGTH 1024

typedef struct {
    char name[MAX_STRING_LENGTH];
    char action[MAX_STRING_LENGTH];
    int duration;
    int is_variable;
    char sounds[MAX_SOUNDS][MAX_STRING_LENGTH];
    int sound_count;
} Step;

typedef struct {
    char name[MAX_STRING_LENGTH];
    int repeats;
    Step steps[MAX_STEPS];
    int step_count;
} Phase;

typedef struct {
    char short_name[MAX_STRING_LENGTH];
    char name[MAX_STRING_LENGTH];
    char description[MAX_STRING_LENGTH];
    char version[MAX_STRING_LENGTH];
    char schematic_version[MAX_STRING_LENGTH];
    char category[MAX_STRING_LENGTH];
    char origin[MAX_STRING_LENGTH];
    char difficulty[MAX_STRING_LENGTH];
    int total_rounds;
    Phase phases[MAX_PHASES];
    int phase_count;
} Practice;

int initialize_or_update_sadhana_hub(void);
int practices_initialized(void);
int get_practices(char practices[][MAX_PRACTICE_NAME], int max_practices);
int load_practice(const char *practice_name, Practice *practice);
int execute_practice(Practice *practice, void (*step_callback)(const Step*, int, int, int, void*), void *user_data);
int configure_practice(const char *practice_name, const char *config_key, const char *config_value);

#endif // CORE_H

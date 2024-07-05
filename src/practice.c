
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "practice.h"
#include "config.h"
#include "utils.h"

#define SADHANA_DIR "~/.local/share/sadhana"
#define REPO_URL "https://github.com/naoxio/sadhana.git"

int install_sadhana(void) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "git clone %s %s", REPO_URL, SADHANA_DIR);
    return system(cmd);
}

int uninstall_sadhana(void) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "rm -rf %s", SADHANA_DIR);
    return system(cmd);
}

int list_practices(void) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "ls %s/practices/*.yaml", SADHANA_DIR);
    return system(cmd);
}

int run_practice(const char *practice_name) {
    // Implement practice execution logic
    printf("Running practice: %s\n", practice_name);
    return 0;
}

int update_practices(void) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "cd %s && git pull", SADHANA_DIR);
    return system(cmd);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "practice.h"
#include "config.h"

void print_help(const char *program_name) {
    printf("Usage: %s <command> [args...]\n\n", program_name);
    printf("Commands:\n");
    printf("  install            Install Sadhana\n");
    printf("  uninstall          Uninstall Sadhana\n");
    printf("  list               List available practices\n");
    printf("  run <practice>     Run a specific practice\n");
    printf("  update             Update practices\n");
    printf("  config <practice>  Configure a specific practice\n");
    printf("  gui                Launch the graphical user interface\n");
    printf("  help               Display this help message\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "help") == 0) {
        print_help(argv[0]);
        return 0;
    } else if (strcmp(argv[1], "install") == 0) {
        return install_sadhana();
    } else if (strcmp(argv[1], "uninstall") == 0) {
        return uninstall_sadhana();
    } else if (strcmp(argv[1], "list") == 0) {
        return list_practices();
    } else if (strcmp(argv[1], "run") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: %s run <practice_name>\n", argv[0]);
            return 1;
        }
        return run_practice(argv[2]);
    } else if (strcmp(argv[1], "update") == 0) {
        return update_practices();
    } else if (strcmp(argv[1], "config") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: %s config <practice_name>\n", argv[0]);
            return 1;
        }
        return configure_practice(argv[2]);
    } else if (strcmp(argv[1], "gui") == 0) {
        return run_gui();
    } else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        print_help(argv[0]);
        return 1;
    }
}

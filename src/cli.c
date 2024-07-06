// cli.c
#include <stdio.h>
#include <string.h>
#include "cli.h"
#include "practice_manager.h"

int list_practices(void) {
    char practices[MAX_PRACTICES][MAX_PRACTICE_NAME];
    int count = get_practices(practices, MAX_PRACTICES);

    if (count < 0) {
        fprintf(stderr, "Failed to get practices\n");
        return 1;
    }

    printf("Available practices:\n");
    for (int i = 0; i < count; i++) {
        printf("- %s\n", practices[i]);
    }

    return 0;
}
void print_help(const char *program_name) {
    printf("Usage: %s <command> [args...]\n\n", program_name);
    printf("Commands:\n");
    printf("  init               Initialize sadhana hub practices\n");
    printf("  update             Update practices\n");
    printf("  list               List available practices\n");
    printf("  run <practice>     Run a specific practice\n");
    printf("  config <practice>  Configure a specific practice\n");
    printf("  help               Display this help message\n");
}

int handle_cli_command(int argc, char *argv[]) {
    if (strcmp(argv[1], "help") == 0) {
        print_help(argv[0]);
        return 0;
    } else if (strcmp(argv[1], "init") == 0) {
        return initialize_practices();
    } else if (strcmp(argv[1], "update") == 0) {
        return update_practices();
    } else if (strcmp(argv[1], "list") == 0) {
        if (!practices_initialized()) {
            printf("Practices not initialized. Would you like to initialize them? (y/n): ");
            char c;
            if (scanf(" %c", &c) == 1 && (c == 'y' || c == 'Y')) {
                int result = initialize_practices();
                if (result != 0) {
                    fprintf(stderr, "Failed to initialize practices.\n");
                    return result;
                }
            } else {
                printf("Practices not initialized. Cannot list.\n");
                return 1;
            }
        }
        return list_practices();
    } else if (strcmp(argv[1], "run") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: %s run <practice_name>\n", argv[0]);
            return 1;
        }
        return run_practice(argv[2]);
    } else if (strcmp(argv[1], "config") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: %s config <practice_name>\n", argv[0]);
            return 1;
        }
        return configure_practice(argv[2]);
    } else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        print_help(argv[0]);
        return 1;
    }
}

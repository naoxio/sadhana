// cli.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "cli.h"
#include "core.h"

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


void cli_step_callback(const Step *step, int round, int phase, int rep, void *user_data) {
    printf("Round %d, Phase %d, Repetition %d\n", round, phase + 1, rep);
    printf("Action: %s\n", step->action);
    
    if (step->is_variable) {
        printf("Duration: Variable (Press Enter when done)\n");
        getchar(); // Wait for user input
    } else {
        printf("Duration: %d seconds\n", step->duration);
        sleep(step->duration);
    }

    if (step->sound_count > 0) {
        printf("Sound: %s\n", step->sounds[0]);
        // In a real implementation, you would play the sound here
    }

    printf("\n");
}

int run_practice_cli(const char *practice_name) {
    Practice practice;
    if (load_practice(practice_name, &practice) != 0) {
        fprintf(stderr, "Failed to load practice: %s\n", practice_name);
        return 1;
    }

    printf("Running practice: %s\n", practice.name);
    printf("Total rounds: %d\n\n", practice.total_rounds);

    execute_practice(&practice, cli_step_callback, NULL);

    printf("Practice completed!\n");
    return 0;
}

int configure_practice_cli(const char *practice_name) {
    char config_key[MAX_STRING_LENGTH];
    char config_value[MAX_STRING_LENGTH];

    printf("Enter configuration key: ");
    if (fgets(config_key, sizeof(config_key), stdin) == NULL) {
        return 1;
    }
    config_key[strcspn(config_key, "\n")] = 0; // Remove newline

    printf("Enter configuration value: ");
    if (fgets(config_value, sizeof(config_value), stdin) == NULL) {
        return 1;
    }
    config_value[strcspn(config_value, "\n")] = 0; // Remove newline

    if (configure_practice(practice_name, config_key, config_value) != 0) {
        fprintf(stderr, "Failed to configure practice: %s\n", practice_name);
        return 1;
    }

    printf("Practice configuration updated successfully.\n");
    return 0;
}


int handle_cli_command(int argc, char *argv[]) {
    if (strcmp(argv[1], "help") == 0) {
        print_help(argv[0]);
        return 0;
    } else if (strcmp(argv[1], "init") == 0 || strcmp(argv[1], "update") == 0) {
        int result = initialize_or_update_sadhana_hub();
        if (result == 0) {
            printf("Practices have been successfully initialized/updated.\n");
        } else {
            fprintf(stderr, "Failed to initialize/update practices.\n");
        }
        return result;
    } else if (strcmp(argv[1], "list") == 0) {
        if (!practices_initialized()) {
            printf("Practices not initialized. Would you like to initialize them? (y/n): ");
            char c;
            if (scanf(" %c", &c) == 1 && (c == 'y' || c == 'Y')) {
                int result = initialize_or_update_sadhana_hub();
                if (result != 0) {
                    fprintf(stderr, "Failed to initialize practices.\n");
                    return result;
                }
                printf("Practices have been successfully initialized.\n");
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
        return run_practice_cli(argv[2]);
    } else if (strcmp(argv[1], "config") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: %s config <practice_name>\n", argv[0]);
            return 1;
        }
        return configure_practice_cli(argv[2]);
    } else {
        fprintf(stderr, "Unknown command: %s\n", argv[1]);
        print_help(argv[0]);
        return 1;
    }
}

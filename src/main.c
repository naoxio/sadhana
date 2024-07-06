#include <string.h>
#include "cli.h"
#include "tui.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "help") == 0) {
        print_help(argv[0]);
        return 0;
    } else if (strcmp(argv[1], "gui") == 0) {
        return run_tui();
    } else {
        return handle_cli_command(argc, argv);
    }
}

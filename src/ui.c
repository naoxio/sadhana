#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termbox.h>
#include "ui.h"
#include "practice.h"
#include "utils.h"

#define MENU_ITEMS 5
#define MAX_PRACTICES 100
#define MAX_PRACTICE_NAME 256

static const char *menu_items[MENU_ITEMS] = {
    "List Practices", "Run Practice", "Update Practices", "Configure Practice", "Exit"
};

static int show_initialization_prompt(void) {
    tb_clear();
    const char *prompt = "Practices not initialized. Would you like to initialize them?";
    const char *yes_option = "[Y] Yes";
    const char *no_option = "[N] No";

    int width = tb_width();
    int height = tb_height();

    tb_print((width - strlen(prompt)) / 2, height / 2 - 1, TB_WHITE, TB_DEFAULT, prompt);
    tb_print((width - strlen(yes_option) - strlen(no_option) - 4) / 2, height / 2 + 1, TB_WHITE, TB_DEFAULT, yes_option);
    tb_print((width + strlen(yes_option) - strlen(no_option) + 4) / 2, height / 2 + 1, TB_WHITE, TB_DEFAULT, no_option);

    tb_present();

    struct tb_event ev;
    while (tb_poll_event(&ev)) {
        if (ev.type == TB_EVENT_KEY) {
            if (ev.ch == 'y' || ev.ch == 'Y') return 1;
            if (ev.ch == 'n' || ev.ch == 'N') return 0;
        }
    }
    return 0;
}

void show_message(const char *message) {
    tb_clear();
    int width = tb_width();
    int height = tb_height();
    tb_print((width - strlen(message)) / 2, height / 2, TB_WHITE, TB_DEFAULT, message);
    tb_print((width - 21) / 2, height / 2 + 2, TB_WHITE, TB_DEFAULT, "Press any key to continue");
    tb_present();

    struct tb_event ev;
    while (tb_poll_event(&ev)) {
        if (ev.type == TB_EVENT_KEY) {
            break;
        }
    }
}

int list_practices_gui(void) {
    tb_clear();
    int width = tb_width();
    int height = tb_height();
    tb_print((width - 17) / 2, 1, TB_WHITE | TB_BOLD, TB_DEFAULT, "List of Practices");

    char practices[MAX_PRACTICES][MAX_PRACTICE_NAME];
    int practice_count = get_practices(practices, MAX_PRACTICES);

    if (practice_count < 0) {
        show_message("Failed to get practices");
        return 1;
    }

    int y = 3;
    for (int i = 0; i < practice_count && y < height - 2; i++) {
        tb_print(2, y++, TB_WHITE, TB_DEFAULT, practices[i]);
    }

    tb_print((width - 21) / 2, height - 1, TB_WHITE, TB_DEFAULT, "Press any key to continue");
    tb_present();

    struct tb_event ev;
    while (tb_poll_event(&ev)) {
        if (ev.type == TB_EVENT_KEY) {
            break;
        }
    }

    return 0;
}

static void draw_menu(int selected) {
    int width = tb_width();
    int height = tb_height();
    tb_clear();
    tb_select_input_mode(TB_INPUT_ESC | TB_INPUT_MOUSE);

    // Draw title
    const char *title = "Sadhana";
    int title_x = (width - strlen(title)) / 2;
    tb_print(title_x, 1, TB_WHITE | TB_BOLD, TB_DEFAULT, title);

    // Draw menu items
    for (int i = 0; i < MENU_ITEMS; i++) {
        int y = 3 + i;
        int x = (width - strlen(menu_items[i])) / 2;
        uint16_t fg = (i == selected) ? TB_BLACK : TB_WHITE;
        uint16_t bg = (i == selected) ? TB_WHITE : TB_DEFAULT;
        tb_print(x, y, fg, bg, menu_items[i]);
    }

    tb_present();
}

int run_gui(void) {
    int selected = 0;
    struct tb_event ev;

    if (tb_init() != 0) {
        fprintf(stderr, "TB_INIT failed\n");
        return 1;
    }
    draw_menu(selected);

    while (1) {
        if (tb_poll_event(&ev) != 0) {
            switch (ev.type) {
                case TB_EVENT_KEY:
                    if (ev.key == TB_KEY_ESC || ev.key == TB_KEY_CTRL_C) {
                        tb_shutdown();
                        return 0;
                    } else if (ev.key == TB_KEY_ARROW_UP) {
                        selected = (selected - 1 + MENU_ITEMS) % MENU_ITEMS;
                    } else if (ev.key == TB_KEY_ARROW_DOWN) {
                        selected = (selected + 1) % MENU_ITEMS;
                    } else if (ev.key == TB_KEY_ENTER) {
                        int result;
                        switch (selected) {
                            case 0:
                                if (!practices_initialized()) {
                                    if (show_initialization_prompt()) {
                                        result = initialize_practices();
                                        if (result != 0) {
                                            show_message("Initialization failed");
                                            break;
                                        }
                                    } else {
                                        show_message("Practices not initialized");
                                        break;
                                    }
                                }
                                result = list_practices_gui();
                                if (result != 0) {
                                    show_message("Failed to list practices");
                                }
                                break;
                            case 1:
                                result = run_practice(NULL);  // Implement practice selection
                                break;
                            case 2:
                                result = update_practices();
                                break;
                            case 3:
                                result = configure_practice(NULL);  // Implement practice selection
                                break;
                            case 4:
                                tb_shutdown();
                                return 0;
                        }
                        draw_menu(selected);
                    }
                    break;
                case TB_EVENT_RESIZE:
                    break;
            }
            draw_menu(selected);
        }
    }
}

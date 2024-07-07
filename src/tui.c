#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <termbox.h>
#include "tui.h"
#include "core.h"

#define MENU_ITEMS 4

static const char *menu_items[MENU_ITEMS] = {
     "Run Practice", "Update Practices", "Configure Practice", "Exit"
};

void tb_print(int x, int y, uint16_t fg, uint16_t bg, const char *str) {
    while (*str) {
        tb_change_cell(x++, y, *str++, fg, bg);
    }
}

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

int select_practice_tui(char *selected_practice) {
    tb_clear();
    int width = tb_width();
    int height = tb_height();
    tb_print((width - 17) / 2, 1, TB_WHITE | TB_BOLD, TB_DEFAULT, "Select a Practice");

    char practices[MAX_PRACTICES][MAX_PRACTICE_NAME];
    int practice_count = get_practices(practices, MAX_PRACTICES);

    if (practice_count < 0) {
        show_message("Failed to get practices");
        return 1;
    }

    int selected = 0;
    int y = 3;

    while (1) {
        tb_clear();
        tb_print((width - 17) / 2, 1, TB_WHITE | TB_BOLD, TB_DEFAULT, "Select a Practice");

        for (int i = 0; i < practice_count && y < height - 2; i++) {
            uint16_t fg = (i == selected) ? TB_BLACK : TB_WHITE;
            uint16_t bg = (i == selected) ? TB_WHITE : TB_DEFAULT;
            tb_print(2, y++, fg, bg, practices[i]);
        }

        tb_present();
        y = 3;

        struct tb_event ev;
        if (tb_poll_event(&ev)) {
            if (ev.type == TB_EVENT_KEY) {
                if (ev.key == TB_KEY_ESC) {
                    return 1;
                } else if (ev.key == TB_KEY_ARROW_UP) {
                    selected = (selected - 1 + practice_count) % practice_count;
                } else if (ev.key == TB_KEY_ARROW_DOWN) {
                    selected = (selected + 1) % practice_count;
                } else if (ev.key == TB_KEY_ENTER) {
                    strcpy(selected_practice, practices[selected]);
                    return 0;
                }
            }
        }
    }
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

void draw_practice_screen(Practice *practice, int current_round, int current_phase, int current_step, int countdown) {
    tb_clear();
    int width = tb_width();
    int height = tb_height();

    char info[256];
    snprintf(info, sizeof(info), "Practice: %s - Round %d/%d - Phase: %s", 
             practice->name, current_round, practice->total_rounds, 
             practice->phases[current_phase].name);
    tb_print((width - strlen(info)) / 2, 1, TB_WHITE | TB_BOLD, TB_DEFAULT, info);

    char action[256];
    snprintf(action, sizeof(action), "Action: %s", 
             practice->phases[current_phase].steps[current_step].action);
    tb_print((width - strlen(action)) / 2, 3, TB_WHITE, TB_DEFAULT, action);

    char time_left[256];
    snprintf(time_left, sizeof(time_left), "Time left: %d seconds", countdown);
    tb_print((width - strlen(time_left)) / 2, 5, TB_WHITE, TB_DEFAULT, time_left);

    tb_print((width - 21) / 2, height - 2, TB_WHITE, TB_DEFAULT, "Press 'q' to quit");

    tb_present();
}

int run_practice_tui(const char *practice_name) {
    Practice practice;
    if (load_practice(practice_name, &practice) != 0) {
        return 1;
    }

    if (tb_init() != 0) {
        fprintf(stderr, "TB_INIT failed\n");
        return 1;
    }

    int current_round = 1;
    int current_phase = 0;
    int current_step = 0;
    int countdown;
    time_t start_time, current_time;

    while (current_round <= practice.total_rounds) {
        Phase *phase = &practice.phases[current_phase];
        Step *step = &phase->steps[current_step];

        countdown = step->duration;
        start_time = time(NULL);

        while (countdown > 0) {
            draw_practice_screen(&practice, current_round, current_phase, current_step, countdown);

            struct tb_event ev;
            if (tb_peek_event(&ev, 100) > 0) {
                if (ev.type == TB_EVENT_KEY && (ev.ch == 'q' || ev.key == TB_KEY_ESC)) {
                    tb_shutdown();
                    return 0;
                }
            }

            current_time = time(NULL);
            countdown = step->duration - (int)(current_time - start_time);
        }

        current_step++;
        if (current_step >= phase->step_count) {
            current_step = 0;
            current_phase++;
            if (current_phase >= practice.phase_count) {
                current_phase = 0;
                current_round++;
            }
        }
    }

    tb_shutdown();
    return 0;
}

int run_tui(void) {
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
                        char selected_practice[MAX_PRACTICE_NAME];

                        switch (selected) {
                            case 0:
                                if (!practices_initialized()) {
                                    if (show_initialization_prompt()) {
                                        result = initialize_or_update_sadhana_hub();
                                        if (result != 0) {
                                            show_message("Initialization failed");
                                            break;
                                        }
                                    } else {
                                        show_message("Practices not initialized");
                                        break;
                                    }
                                }
                                result = select_practice_tui(selected_practice);
                                if (result == 0) {
                                    result = run_practice_tui(selected_practice);
                                    if (result != 0) {
                                        show_message("Failed to run practice");
                                    }
                                }
                                break;
                            case 1:
                                result = initialize_or_update_sadhana_hub();
                                break;
                            case 2:
                                result = configure_practice(NULL, "dummy_key", "dummy_value");  // Implement practice selection
                                break;
                            case 3:
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termbox.h>
#include "ui.h"
#include "practice.h"
#include "utils.h"

#define MENU_ITEMS 5

static const char *menu_items[MENU_ITEMS] = {
    "List Practices", "Run Practice", "Update Practices", "Configure Practice", "Exit"
};

static void draw_menu(int selected) {
    int width, height;
    tb_clear();
    tb_select_input_mode(TB_INPUT_ESC | TB_INPUT_MOUSE);
    width = tb_width();
    height = tb_height();

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
                        tb_shutdown();
                        switch (selected) {
                            case 0: return list_recipes();
                            case 1: return run_practice(NULL);  // Implement practice selection
                            case 2: return update_recipes();
                            case 3: return configure_practice(NULL);  // Implement practice selection
                            case 4: return 0;  // Exit
                        }
                    }
                    break;
                case TB_EVENT_RESIZE:
                    break;
            }
            draw_menu(selected);
        }
    }
}

#ifndef UI_H
#define UI_H

#include <termbox.h>

// Function prototypes
int run_tui(void);
void show_message(const char *message);
void tb_print(int x, int y, uint16_t fg, uint16_t bg, const char *str);
int select_practices_tui(void);
int run_practice_tui(const char *practice_name);

#endif // UI_H

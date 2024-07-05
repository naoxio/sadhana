
#include <string.h>
#include <termbox.h>
#include "utils.h"

void tb_print(int x, int y, uint16_t fg, uint16_t bg, const char *str) {
    while (*str) {
        tb_change_cell(x++, y, *str++, fg, bg);
    }
}

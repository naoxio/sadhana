#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <termbox.h>

void tb_print(int x, int y, uint16_t fg, uint16_t bg, const char *str);

#endif

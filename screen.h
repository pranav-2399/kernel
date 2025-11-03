#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

void vga_put_char(char c);
void vga_print(const char *str);
void vga_print_prompt();

#endif

#include <stdint.h>
#include "ports.h"
#define VGA_ADDRESS 0xB8000
#define WHITE_ON_BLACK 0x0F

// optional: a simple US keyboard scancode to ASCII map
unsigned char keyboard_map[128] =
{
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t', 'q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`',  0,'\\','z',
    'x','c','v','b','n','m',',','.','/',  0, '*', 0, ' '
};

// sending IRQ - Interrupt ReQuest
extern unsigned short *vga_buffer;
int cursor_row = 5;
int cursor_col = 0;

unsigned char scancode_to_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ', 0
};

static inline void vga_put_char(char c) {
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
        return;
    }

    vga_buffer[cursor_row * 80 + cursor_col] = (unsigned short)(c | (WHITE_ON_BLACK << 8));
    cursor_col++;
    if (cursor_col >= 80) {
        cursor_col = 0;
        cursor_row++;
    }
}

void keyboard_handler() {
    unsigned char scancode = inb(0x60);

    if (!(scancode & 0x80)) {  // if key pressed (not released)
        char c = scancode_to_ascii[scancode];
        if (c) vga_put_char(c);
    }

    // send the end of interrupt - EOI
    outb(0x20, 0x20);
}

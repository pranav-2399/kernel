#include <stdint.h>
#include "ports.h"
#include "commands.h"   // well add this later

#define VGA_ADDRESS 0xB8000
#define WHITE_ON_BLACK 0x0F

extern unsigned short *vga_buffer;

#define MAX_INPUT 128

int cursor_row = 15;
int cursor_col = 0;
int new_prompt_needed = 1;
int shift_pressed = 0;

static char input_buffer[MAX_INPUT];
static int input_index = 0;

// Lowercase map
unsigned char scancode_to_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ', 0
};

// Uppercase map
unsigned char scancode_to_ascii_shifted[128] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
    'A','S','D','F','G','H','J','K','L',':','"','~', 0, '|',
    'Z','X','C','V','B','N','M','<','>','?', 0, '*', 0, ' ', 0
};

void vga_backspace() {
    if (cursor_col > 0) {
        cursor_col--;
        vga_buffer[cursor_row * 80 + cursor_col] = (unsigned short)(' ' | (WHITE_ON_BLACK << 8));
    }
}

void vga_put_char(char c) {
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
        new_prompt_needed = 1;
    } else {
        vga_buffer[cursor_row * 80 + cursor_col] = (unsigned short)(c | (WHITE_ON_BLACK << 8));
        cursor_col++;
    }

    if (cursor_row >= 25) {
        for (int r = 1; r < 25; r++)
            for (int c = 0; c < 80; c++)
                vga_buffer[(r - 1) * 80 + c] = vga_buffer[r * 80 + c];
        for (int c = 0; c < 80; c++)
            vga_buffer[24 * 80 + c] = (unsigned short)(' ' | (WHITE_ON_BLACK << 8));
        cursor_row = 24;
    }
}

void vga_print_prompt() {
    const char *prompt = ">>> ";
    for (int i = 0; prompt[i]; i++) {
        vga_put_char(prompt[i]);
    }
    new_prompt_needed = 0;
}

// =============== MAIN HANDLER ===============
void keyboard_handler() {
    unsigned char scancode = inb(0x60);

    // SHIFT handling
    if (scancode == 0x2A || scancode == 0x36) { shift_pressed = 1; outb(0x20, 0x20); return; }
    if (scancode == 0xAA || scancode == 0xB6) { shift_pressed = 0; outb(0x20, 0x20); return; }

    // Key press only
    if (!(scancode & 0x80)) {
        if (new_prompt_needed) vga_print_prompt();

        char c = shift_pressed ? scancode_to_ascii_shifted[scancode] : scancode_to_ascii[scancode];

        if (c == '\b') {
            if (input_index > 0) {
                input_index--;
                input_buffer[input_index] = '\0';
                vga_backspace();
            }
        }
        else if (c == '\n') {
            vga_put_char('\n');
            input_buffer[input_index] = '\0';
            handle_command(input_buffer);   // <=== HERE is where Enter key is handled
            input_index = 0;
            new_prompt_needed = 1;
        }
        else if (c) {
            if (input_index < MAX_INPUT - 1) {
                input_buffer[input_index++] = c;
                vga_put_char(c);
            }
        }
    }

    // End Of Interrupt
    outb(0x20, 0x20);
}

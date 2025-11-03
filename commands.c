#include "commands.h"
#include "string.h"

void vga_print(const char *str); // declare it here

void handle_command(const char *input) {
    if (input[0] == '\0') return; // ignore empty commands

    if (strncmp(input, "print ", 6) == 0) {
        const char *message = input + 6;
        vga_print(message);
        vga_put_char('\n');  // move to new line after print
    } else {
        vga_print("Unknown command\n");
    }
}

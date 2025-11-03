#include "commands.h"
#include "string.h"
#include "screen.h"

void handle_command(const char *input) {
    if (input[0] == '\0') return;

    if (strncmp(input, "print ", 6) == 0) {
        const char *message = input + 6;
        vga_print(message);
        vga_put_char('\n');
    } 
    else if (strncmp(input, "divide ", 7) == 0) {
        int a = 0, b = 0;
        const char *args = input + 7;
        const char *slash = strchr(args, '/');

        if (!slash) {
            vga_print("Usage: divide a/b\n");
            return;
        }

        a = atoi(args);
        b = atoi(slash + 1);

        if (b == 0) {
            vga_print("Dividing by zero...\n");
            int result = a / b;
            (void)result;
        } else {
            int result = a / b;
            vga_print("Result: ");
            char buf[12];
            itoa(result, buf, 10);
            vga_print(buf);
            vga_put_char('\n');
        }
    }

    else {
        vga_print("Unknown command\n");
    }
}

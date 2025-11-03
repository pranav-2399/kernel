#include "ports.h"
#include "interrupts.h"
#include "kernel_threads.h"
#include "screen.h"

# define MAX_THREADS 16
# define STACK_SIZE 4096

#define VGA_ADDRESS 0xB8000
unsigned short *vga_buffer = (unsigned short *)VGA_ADDRESS;

typedef struct {
    thread_func func;
    int priority;
    int finished;
} thread_t;

volatile thread_t threads[MAX_THREADS];
volatile int num_threads = 0;
volatile int current_thread = 0;

void kernel_thread_finish(){ threads[current_thread].finished = 1;}

int register_thread(thread_func func, int priority){
    if (num_threads >= MAX_THREADS) return -1;
    threads[num_threads].func = func;
    threads[num_threads].priority = priority;
    threads[num_threads].finished = 0;
    num_threads++;
    return 0;
}

void schedule() {
    int highest_priority = -1;
    int next = -1;

    for (int i = 0; i < num_threads; i++) {
        if (!threads[i].finished && threads[i].priority > highest_priority) {
            highest_priority = threads[i].priority;
            next = i;
        }
    }

    if (next != -1){ current_thread = next;}
}

void scheduler_tick() {
    if (num_threads == 0){ return;}
    if (!threads[current_thread].finished){
      threads[current_thread].func();
    }
    schedule();
}

#define VGA_ADDRESS 0xB8000
#define YELLOW 14
#define RED    4
#define WHITE_ON_BLACK 0x0F

void vga_print(const char *str) {
    // print starting from wherever the cursor currently is
    for (int i = 0; str[i]; i++) {
        vga_put_char(str[i]);
    }
}

void vga_print_hex(uint32_t num) {
    const char *hex_digits = "0123456789ABCDEF";
    char hex_str[11];  // "0x" + 8 digits + null terminator
    hex_str[0] = '0';
    hex_str[1] = 'x';

    for (int i = 0; i < 8; i++) {
        uint8_t nibble = (num >> ((7 - i) * 4)) & 0xF;
        hex_str[i + 2] = hex_digits[nibble];
    }
    hex_str[10] = '\0';

    vga_print(hex_str);
}

void kernel_main() {
    // Initialize VGA
    vga_print("Kernel Starting...\n");

    // Setup interrupts
    pic_remap();
    idt_install();
    timer_install();
    
    // Enable keyboard
    outb(0x21, inb(0x21) & ~0x02);
    asm volatile("sti");

    //volatile int c = 1 / 0;  // This will cause exception

    // Should never reach here
    while(1) {
        asm volatile("hlt");
    }
}

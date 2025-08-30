#include <stdint.h>

#define VGA_ADDRESS 0xB8000
#define YELLOW 14
#define RED    4

unsigned short *vga_buffer = (unsigned short *)VGA_ADDRESS;

/* Global counters to track thread activity */
volatile int counter1 = 0;
volatile int counter2 = 0;

/* Simple VGA print function to a specific row */
void vga_print(const char *str, unsigned char color, int row) {
    int index = row * 80;
    while (*str) {
        vga_buffer[index++] = (*str) | (color << 8);
        str++;
    }
}

/* --- Thread functions --- */
void thread1() {
    counter1++;
    char msg[20];
    int n = counter1 % 1000; // keep number small
    vga_print("Thread 1 running: ", YELLOW, 10);
    // print counter as digit
    vga_buffer[10*80 + 18] = '0' + (n / 100) % 10 | (YELLOW << 8);
    vga_buffer[10*80 + 19] = '0' + (n / 10) % 10 | (YELLOW << 8);
    vga_buffer[10*80 + 20] = '0' + n % 10 | (YELLOW << 8);
}

void thread2() {
    counter2++;
    int n = counter2 % 1000;
    vga_print("Thread 2 running: ", RED, 11);
    vga_buffer[11*80 + 18] = '0' + (n / 100) % 10 | (RED << 8);
    vga_buffer[11*80 + 19] = '0' + (n / 10) % 10 | (RED << 8);
    vga_buffer[11*80 + 20] = '0' + n % 10 | (RED << 8);
}

/* --- Scheduler --- */
typedef void (*thread_func)();
thread_func threads[2];
int current_thread = 0;

void scheduler_tick() {
    current_thread = (current_thread + 1) % 2;
}

/* --- Kernel entry --- */
void kernel_main() {
    vga_print("Kernel Started!", YELLOW, 0);

    /* Initialize thread array */
    threads[0] = thread1;
    threads[1] = thread2;

    /* Main loop: cooperative round-robin scheduler */
    while (1) {
        threads[current_thread]();  // run current thread
        scheduler_tick();           // switch to next thread
        for (volatile int i=0; i<1000000; i++); // small delay
    }
}

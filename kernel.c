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


void kernel_main() {
    vga_print("Popcorn");

    pic_remap();
    idt_install();
    timer_install();

    // unmask (enable) keyboard 
    // interrupt request - setting bit 1 to 0
    outb(0x21, inb(0x21) & ~0x02);

    asm volatile("sti");
    extern void vga_put_char(char);
    extern void vga_print_prompt();

    vga_print_prompt(); // printing the 1st prompt manually 

    while (1) { asm volatile("hlt"); }

}

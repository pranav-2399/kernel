#include <stdint.h>
#include "ports.h"

/* VGA definitions */
#define VGA_ADDRESS 0xB8000
#define YELLOW 14
#define RED    4
#define WHITE_ON_BLACK 0x0F

unsigned short *vga_buffer = (unsigned short *)VGA_ADDRESS;

/* Global counters */
volatile int counter1 = 0;
volatile int counter2 = 999;

/* VGA print */
void vga_print(const char *str, unsigned char color, int row) {
    int index = row * 80;
    while (*str) {
        vga_buffer[index++] = (*str) | (color << 8);
        str++;
    }
}

static inline uint16_t get_cs(void) {
    uint16_t cs;
    __asm__ volatile ("mov %%cs, %0" : "=r"(cs));
    return cs;
}

/* Thread functions */
void thread1() {
    counter1++;
    int n = counter1 % 1000;
    vga_print("Thread 1 (Up) running: ", YELLOW, 10);
    vga_buffer[10*80 + 18] = (('0' + (n / 100) % 10) | (YELLOW << 8));
    vga_buffer[10*80 + 19] = (('0' + (n / 10) % 10) | (YELLOW << 8));
    vga_buffer[10*80 + 20] = (('0' + n % 10) | (YELLOW << 8));
}

void thread2() {
    counter2--;
    int n = counter2 % 1000;
    if (n < 0) n+= 1000;
    vga_print("Thread 2 (Down) running: ", RED, 11);
    vga_buffer[11*80 + 18] = (('0' + (n / 100) % 10) | (RED << 8));
    vga_buffer[11*80 + 19] = (('0' + (n / 10) % 10) | (RED << 8));
    vga_buffer[11*80 + 20] = (('0' + n % 10) | (RED << 8));
}

/* Scheduler */
typedef void (*thread_func)();
thread_func threads[2];
int current_thread = 0;

/* Called by the timer interrupt handler */
void scheduler_tick() {
    threads[current_thread]();              // run current thread
    current_thread = (current_thread + 1) % 2;
}

/* --- IDT setup --- */
struct idt_entry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

#define IDT_SIZE 256
struct idt_entry idt[IDT_SIZE];
struct idt_ptr idtp;

extern void idt_load(struct idt_ptr*);    // defined in assembly
extern void timer_handler(); // ISR from assembly

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags) {
    idt[num].base_low = (base & 0xFFFF);
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
    idt[num].base_high = (base >> 16) & 0xFFFF;
}

void idt_install() {
    idtp.limit = (sizeof(struct idt_entry) * IDT_SIZE) - 1;
    idtp.base = (uint32_t)&idt;

    for (int i = 0; i < IDT_SIZE; i++) {
        idt[i].base_low = 0;
        idt[i].sel = 0;
        idt[i].always0 = 0;
        idt[i].flags = 0;
        idt[i].base_high = 0;
    }

    idt_set_gate(32, (unsigned)timer_handler, get_cs(), 0x8E);
    idt_load(&idtp);
}

/* --- PIC Remap --- */
void pic_remap() {
    /* send ICW1 */
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    /* ICW2: master PIC vector offset */
    outb(0x21, 0x20);
    /* ICW2: slave PIC vector offset */
    outb(0xA1, 0x28);
    /* ICW3 */
    outb(0x21, 4);
    outb(0xA1, 2);
    /* ICW4 */
    outb(0x21, 1);
    outb(0xA1, 1);
    /* mask interrupts */
    outb(0x21, 0xFE);
    outb(0xA1, 0xFF);
}

/* PIT setup: ~100Hz */
void timer_install() {
    unsigned int divisor = 1193180 / 20;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

/* Kernel entry */
void kernel_main() {
    vga_print("Kernel with Preemptive Threads", YELLOW, 0);

    threads[0] = thread1;
    threads[1] = thread2;

    pic_remap();
    idt_install();
    timer_install();

    asm volatile("sti"); // enable interrupts

    while (1) {
        asm volatile("hlt"); // halt CPU until next interrupt
    }

    /* int n = 0;
    while (1) {
        vga_buffer[10*80 + 18] = '0' + (n % 10) | (0x0E << 8);
        n++;
        for (volatile int i = 0; i < 1000000; i++); // slow down
    } */
}

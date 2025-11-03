#include <stdint.h>
#include "ports.h"

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

# define IDT_SIZE 256
struct idt_entry idt[IDT_SIZE];
struct idt_ptr idtp;

extern void idt_load(struct idt_ptr*);
extern void timer_handler();

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags) {
    idt[num].base_low = (base & 0xFFFF);
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
    idt[num].base_high = (base >> 16) & 0xFFFF;
}

static inline uint16_t get_cs(void) { // get code segment
    uint16_t cs;
    __asm__ volatile ("mov %%cs, %0" : "=r"(cs));
    return cs;
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
    extern void keyboard_handler_stub();
    idt_set_gate(33, (unsigned)keyboard_handler_stub, get_cs(), 0x8E);
    idt_set_gate(32, (unsigned)timer_handler, get_cs(), 0x8E);
    idt_load(&idtp);
}

void pic_remap() {
    // send ICW 1
    outb(0x20, 0x11); outb(0xA0, 0x11);
    /* ICW2: master PIC vector offset */
    outb(0x21, 0x20);
    /* ICW2: slave PIC vector offset */
    outb(0xA1, 0x28);
    /* ICW3 */
    outb(0x21, 4); outb(0xA1, 2);
    /* ICW4 */
    // setting it to normal protected mode (normal 8086/88)
    outb(0x21, 1); outb(0xA1, 1);
    
    // mask interrupts
    outb(0x21, 0xFC);  // 11111100 — enables IRQ0 (timer) and IRQ1 (keyboard)
    outb(0xA1, 0xFF);  // mask all slave IRQs
}

void timer_install() {
    unsigned int divisor = 1193180 / 20;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}
#include <stdint.h>
#include "ports.h"
#include "screen.h"
#include "isr.h"

/* --- External assembly stubs and C handlers (defined elsewhere) --- */
extern void idt_load(void*);            // implemented in isr.asm (idt_load expects pointer to idt_ptr)
extern void timer_handler(void);        // timer IRQ stub in isr.asm
extern void keyboard_handler_stub(void); // keyboard IRQ stub in isr.asm

void isr_handler(registers_t regs);

void isr_handler(registers_t regs) {
    vga_print("Received interrupt: ");
    vga_print_hex(regs.int_no);
    vga_print("\n");

    // Specific exception
    if (regs.int_no == 0) {
        vga_print("Divide by zero error!\n");
    }

    // For testing, halt only on CPU exceptions
    if (regs.int_no < 32) {
        vga_print("Exception occurred. Halting...\n");
        for (;;)
            __asm__ __volatile__("hlt");
    }
}


/* CPU exception stubs (defined in isr.asm) */
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

/* ------------------------------------------------------------------ */
/* IDT structures                                                      */
/* ------------------------------------------------------------------ */

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
static struct idt_entry idt[IDT_SIZE];
static struct idt_ptr idtp;

/* ------------------------------------------------------------------ */
/* Helper to set a single IDT entry                                  */
/* base is the 32-bit handler address                                 */
/* sel is code segment selector (from get_cs())                      */
/* flags: present(1)<<7 | DPL<<5 | type (0xE for 32-bit interrupt gate) */
/* ------------------------------------------------------------------ */
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = (uint16_t)(base & 0xFFFF);
    idt[num].sel       = sel;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
    idt[num].base_high = (uint16_t)((base >> 16) & 0xFFFF);
}

/* ------------------------------------------------------------------ */
/* Fetch current code segment selector                                */
/* ------------------------------------------------------------------ */
static inline uint16_t get_cs(void) {
    uint16_t cs;
    __asm__ volatile("mov %%cs, %0" : "=r"(cs));
    return cs;
}

/* ------------------------------------------------------------------ */
/* Install IDT: fill entries and load with lidt                       */
/* ------------------------------------------------------------------ */
void idt_install(void) {
    idtp.limit = (sizeof(struct idt_entry) * IDT_SIZE) - 1;
    idtp.base  = (uint32_t)&idt;

    /* clear IDT */
    for (int i = 0; i < IDT_SIZE; ++i) {
        idt[i].base_low  = 0;
        idt[i].sel       = 0;
        idt[i].always0   = 0;
        idt[i].flags     = 0;
        idt[i].base_high = 0;
    }

    uint16_t cs = get_cs();

    /* --- CPU exceptions 0..31 --- */
    idt_set_gate(0,  (uint32_t)isr0,  cs, 0x8E);
    idt_set_gate(1,  (uint32_t)isr1,  cs, 0x8E);
    idt_set_gate(2,  (uint32_t)isr2,  cs, 0x8E);
    idt_set_gate(3,  (uint32_t)isr3,  cs, 0x8E);
    idt_set_gate(4,  (uint32_t)isr4,  cs, 0x8E);
    idt_set_gate(5,  (uint32_t)isr5,  cs, 0x8E);
    idt_set_gate(6,  (uint32_t)isr6,  cs, 0x8E);
    idt_set_gate(7,  (uint32_t)isr7,  cs, 0x8E);
    idt_set_gate(8,  (uint32_t)isr8,  cs, 0x8E);
    idt_set_gate(9,  (uint32_t)isr9,  cs, 0x8E);
    idt_set_gate(10, (uint32_t)isr10, cs, 0x8E);
    idt_set_gate(11, (uint32_t)isr11, cs, 0x8E);
    idt_set_gate(12, (uint32_t)isr12, cs, 0x8E);
    idt_set_gate(13, (uint32_t)isr13, cs, 0x8E);
    idt_set_gate(14, (uint32_t)isr14, cs, 0x8E);
    idt_set_gate(15, (uint32_t)isr15, cs, 0x8E);
    idt_set_gate(16, (uint32_t)isr16, cs, 0x8E);
    idt_set_gate(17, (uint32_t)isr17, cs, 0x8E);
    idt_set_gate(18, (uint32_t)isr18, cs, 0x8E);
    idt_set_gate(19, (uint32_t)isr19, cs, 0x8E);
    idt_set_gate(20, (uint32_t)isr20, cs, 0x8E);
    idt_set_gate(21, (uint32_t)isr21, cs, 0x8E);
    idt_set_gate(22, (uint32_t)isr22, cs, 0x8E);
    idt_set_gate(23, (uint32_t)isr23, cs, 0x8E);
    idt_set_gate(24, (uint32_t)isr24, cs, 0x8E);
    idt_set_gate(25, (uint32_t)isr25, cs, 0x8E);
    idt_set_gate(26, (uint32_t)isr26, cs, 0x8E);
    idt_set_gate(27, (uint32_t)isr27, cs, 0x8E);
    idt_set_gate(28, (uint32_t)isr28, cs, 0x8E);
    idt_set_gate(29, (uint32_t)isr29, cs, 0x8E);
    idt_set_gate(30, (uint32_t)isr30, cs, 0x8E);
    idt_set_gate(31, (uint32_t)isr31, cs, 0x8E);

    /* --- Hardware IRQs (remapped) --- */
    idt_set_gate(32, (uint32_t)timer_handler,         cs, 0x8E);
    idt_set_gate(33, (uint32_t)keyboard_handler_stub, cs, 0x8E);

    /* load IDT */
    idt_load(&idtp);
}

/* ------------------------------------------------------------------ */
/* PIC remapping and IRQ mask helpers                                 */
/* ------------------------------------------------------------------ */
void pic_remap(void) {
    /* start init */
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    /* set vector offset */
    outb(0x21, 0x20); /* master offset 0x20 (32) */
    outb(0xA1, 0x28); /* slave  offset 0x28 (40) */
    /* setup cascading */
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    /* environment info (8086 mode) */
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    /* mask IRQs: keep IRQ0 and IRQ1 enabled (bits 0 and 1 = 0) */
    outb(0x21, 0xFC); /* 11111100 -> mask all except 0,1 */
    outb(0xA1, 0xFF); /* mask all on slave */
}

/* ------------------------------------------------------------------ */
/* PIT timer programming (keeps your existing timer_install semantics) */
/* ------------------------------------------------------------------ */
void timer_install(void) {
    uint32_t divisor = 1193180 / 20; /* 20 Hz */
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

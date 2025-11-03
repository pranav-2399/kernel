#include <stdint.h>
#include "screen.h"
#include "ports.h"

void panic_halt(const char* message) {
    vga_print("\nKERNEL PANIC: ");
    vga_print(message);
    vga_print("\nSystem halted.\n");

    asm volatile("cli");
    while (1) {
        asm volatile("hlt");
    }
}


static inline uint32_t read_cr2(void) {
    uint32_t val;
    __asm__ volatile("mov %%cr2, %0" : "=r"(val));
    return val;
}


typedef struct regs {
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} regs_t;


static const char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Security Exception",
    "Reserved 22",
    "Reserved 23",
    "Reserved 24",
    "Reserved 25",
    "Reserved 26",
    "Reserved 27",
    "Reserved 28",
    "Reserved 29",
    "Reserved 30",
    "Reserved 31"
};


void vga_print_hex(uint32_t val) {
    const char *hex = "0123456789ABCDEF";
    char s[9];
    s[8] = '\0';
    for (int i = 7; i >= 0; --i) {
        s[i] = hex[val & 0xF];
        val >>= 4;
    }
    vga_print(s);
}


void isr_handler(regs_t *r) {
    vga_print("\n\n[][][] KERNEL EXCEPTION OCCURRED [][][]\n");

    if (r->int_no < 32) {
        vga_print("Exception: ");
        vga_print(exception_messages[r->int_no]);
        vga_print("\n");
    } else {
        vga_print("Unknown Exception\n");
    }

    vga_print("EAX: "); vga_print_hex(r->eax); vga_print("  ");
    vga_print("EBX: "); vga_print_hex(r->ebx); vga_print("\n");

    vga_print("ECX: "); vga_print_hex(r->ecx); vga_print("  ");
    vga_print("EDX: "); vga_print_hex(r->edx); vga_print("\n");

    vga_print("ESI: "); vga_print_hex(r->esi); vga_print("  ");
    vga_print("EDI: "); vga_print_hex(r->edi); vga_print("\n");

    vga_print("EBP: "); vga_print_hex(r->ebp); vga_print("  ");
    vga_print("ESP: "); vga_print_hex(r->esp); vga_print("\n");

    vga_print("INT_NO: "); vga_print_hex(r->int_no);
    vga_print("  ERR_CODE: "); vga_print_hex(r->err_code);
    vga_print("\n");

    vga_print("EIP: "); vga_print_hex(r->eip);
    vga_print("  CS: "); vga_print_hex(r->cs);
    vga_print("  EFLAGS: "); vga_print_hex(r->eflags);
    vga_print("\n");

    if (r->int_no == 14) {
        uint32_t cr2 = read_cr2();
        vga_print("Page Fault at address: ");
        vga_print_hex(cr2);
        vga_print("\n");
    }

    panic_halt("CPU Exception");
}

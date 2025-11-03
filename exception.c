// Improved exception handler: print exception name/number + err code and halt.
// Keeps behavior safe (does not attempt unsafe recovery).
#include "screen.h"
#include "ports.h"
#include <stdint.h>

#define VGA_ADDRESS 0xB8000
static unsigned short *vga_buffer = (unsigned short *)VGA_ADDRESS;

/* Exception messages for 0..31 */
static const char *exception_messages[32] = {
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
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection",
    "VMM Communication",
    "Security Exception",
    "Reserved"
};

/* helper to print decimal number */
static void print_uint(unsigned int v) {
    char buf[12];
    int i = 0;
    if (v == 0) {
        vga_put_char('0');
        return;
    }
    while (v) {
        buf[i++] = '0' + (v % 10);
        v /= 10;
    }
    for (int j = i - 1; j >= 0; --j) vga_put_char(buf[j]);
}

/* Called from assembly ISR stubs: exception_handler(int_no, err_code) */
void exception_handler(unsigned int int_no, unsigned int err_code) {
    // Clear screen first to make message more visible
    for(int i = 0; i < 80*25; i++) {
        vga_buffer[i] = (unsigned short)(' ' | (0x4F << 8));  // White on red background
    }

    vga_print("\n\n**************************\n");
    vga_print("*    CPU EXCEPTION      *\n");
    vga_print("**************************\n\n");
    
    vga_print("Exception detected: ");
    if (int_no < 32) {
        vga_print(exception_messages[int_no]);
    } else {
        vga_print("Unknown");
    }
    vga_print("\n\nError code: ");
    print_uint(err_code);
    
    vga_print("\n\n--- System halted ---\n");
    vga_print("Press Ctrl+Alt+Del to restart\n");
    vga_print("Press Ctrl+C to exit QEMU\n");

    // Disable interrupts and enter infinite loop
    asm volatile("cli");
    while(1) {
        asm volatile("hlt");
    }
}
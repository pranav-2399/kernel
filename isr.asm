[bits 32]
global idt_load
global timer_handler
extern scheduler_tick

section .text

idt_load:
    ; arg: [esp+4] = pointer to idt_ptr
    mov eax, [esp+4]
    lidt [eax]
    ret

timer_handler:
    pusha
    call scheduler_tick
    popa
    mov al, 0x20
    out 0x20, al        ; EOI to master PIC (IRQ0)
    iret

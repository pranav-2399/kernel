[bits 32]
global idt_load
global timer_handler
extern scheduler_tick
global keyboard_handler_stub
extern keyboard_handler

section .text

idt_load:
    mov eax, [esp+4]
    lidt [eax]
    ret

timer_handler:
    pusha
    call scheduler_tick
    popa
    mov al, 0x20
    out 0x20, al
    iret

keyboard_handler_stub:
    pusha
    call keyboard_handler     ; call C function
    mov al, 0x20
    out 0x20, al              ; send EOI
    popa
    iret

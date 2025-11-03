[bits 32]
global idt_load
global timer_handler
extern scheduler_tick
global keyboard_handler_stub
extern keyboard_handler
extern exception_handler

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

; generic exception stubs (0..31). Each pushes an explicit error code 0
; and the interrupt number, then calls exception_handler(int_no, err_code).
%macro EXCEPTION 1
global isr%1
isr%1:
    pusha
    push dword 0        ; err_code (0 for those without a hardware err code)
    push dword %1       ; int number
    call exception_handler
    add esp, 8
    popa
    iret
%endmacro

EXCEPTION 0
EXCEPTION 1
EXCEPTION 2
EXCEPTION 3
EXCEPTION 4
EXCEPTION 5
EXCEPTION 6
EXCEPTION 7
EXCEPTION 8
EXCEPTION 9
EXCEPTION 10
EXCEPTION 11
EXCEPTION 12
EXCEPTION 13
EXCEPTION 14
EXCEPTION 15
EXCEPTION 16
EXCEPTION 17
EXCEPTION 18
EXCEPTION 19
EXCEPTION 20
EXCEPTION 21
EXCEPTION 22
EXCEPTION 23
EXCEPTION 24
EXCEPTION 25
EXCEPTION 26
EXCEPTION 27
EXCEPTION 28
EXCEPTION 29
EXCEPTION 30
EXCEPTION 31
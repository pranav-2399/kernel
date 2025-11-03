; isr.asm - exception + IRQ stubs
; 32-bit protected-mode NASM file

[bits 32]

; --- imports/exports ---
global idt_load
global timer_handler
global keyboard_handler_stub

extern scheduler_tick        ; C function called by timer IRQ stub
extern keyboard_handler     ; C function called by keyboard IRQ stub
extern isr_handler          ; C handler (called by exception stubs)

section .text

; -------------------------
; idt_load: load IDT pointer
; expects a pointer to idt_ptr on the stack (called from C: idt_load(&idtp))
; -------------------------
idt_load:
    mov eax, [esp+4]
    lidt [eax]
    ret

; -------------------------
; Timer IRQ (IRQ0) stub
; calls C scheduler_tick and sends EOI to PIC
; -------------------------
timer_handler:
    pusha
    call scheduler_tick
    popa
    mov al, 0x20
    out 0x20, al     ; send EOI to master PIC
    iret

; -------------------------
; Keyboard IRQ (IRQ1) stub
; calls C keyboard handler and sends EOI
; -------------------------
keyboard_handler_stub:
    pusha
    call keyboard_handler
    popa
    mov al, 0x20
    out 0x20, al
    iret

; -------------------------
; Exception stubs (0..31)
; We create a uniform stack layout for the C handler:
;   push err_code (0 for exceptions without hardware error code)
;   push int_no
;   pusha
;   push ds
;   push es
;   set ds/es to kernel data selector (0x10)
;   call isr_handler
;   restore es/ds
;   popa
;   add esp, <cleanup> ; remove int_no & err_code
;   sti
;   iret
;
; For exceptions that the CPU pushes an error code for (listed below),
; we do NOT push a dummy error code — CPU already pushed it — so cleanup differs.
; -------------------------

%macro ISR_NOERR 1
global isr%1
isr%1:
    cli
    push dword 0          ; dummy error code so handler always finds an err_code
    push dword %1         ; interrupt number
    pusha
    push ds
    push es
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    call isr_handler
    pop es
    pop ds
    popa
    add esp, 8            ; remove int_no + err_code
    sti
    iret
%endmacro

%macro ISR_ERR 1
global isr%1
isr%1:
    cli
    push dword %1         ; interrupt number (CPU already pushed err_code)
    pusha
    push ds
    push es
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    call isr_handler
    pop es
    pop ds
    popa
    add esp, 4            ; remove int_no only (err_code left by CPU)
    sti
    iret
%endmacro

; Exceptions with an error code according to Intel manuals:
; 8, 10, 11, 12, 13, 14, 17
ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR   17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31

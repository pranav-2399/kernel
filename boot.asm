[bits 32]
extern kernel_main
global start

section .multiboot
align 4
    dd 0x1BADB002
    dd 0
    dd -(0x1BADB002)

section .text
start:
    call kernel_main
.hang:
    hlt
    jmp .hang

[bits 32]
global keyboard_handler_stub
extern keyboard_handler

keyboard_handler_stub:
    pusha
    call keyboard_handler
    popa
    iretd

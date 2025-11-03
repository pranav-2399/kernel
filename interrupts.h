#pragma once

void idt_install();
void pic_remap();
void timer_install();

void divide_by_zero_handler();
extern void idt_load(void*);
extern void timer_handler();

#ifndef ISR_H
#define ISR_H

#include <stdint.h>

typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

void isr_handler(registers_t regs);

#endif

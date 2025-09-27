#pragma once

void idt_install();
void pic_remap();
void timer_install();

extern void idt_load(void*);
extern void timer_handler();

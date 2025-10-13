#!/bin/sh
echo "compiling asm files"
nasm -f elf32 boot.asm -o boot.o
nasm -f elf32 isr.asm -o isr.o

echo "compiling c to obj files"
x86_64-elf-gcc -m32 -ffreestanding -fno-pic -fno-pie -O0 -Wall -Wextra -c kernel.c -o kernel.o
x86_64-elf-gcc -m32 -ffreestanding -fno-pic -fno-pie -O0 -Wall -Wextra -c ports.c -o ports.o
x86_64-elf-gcc -m32 -ffreestanding -fno-pic -fno-pie -O0 -Wall -Wextra -c threads.c -o threads.o
x86_64-elf-gcc -m32 -ffreestanding -fno-pic -fno-pie -O0 -Wall -Wextra -c interrupts.c -o interrupts.o
x86_64-elf-gcc -m32 -ffreestanding -fno-pic -fno-pie -O0 -Wall -Wextra -c keyboard.c -o keyboard.o

echo "linker"
x86_64-elf-ld -m elf_i386 -T linker.ld -o kernel.elf boot.o isr.o kernel.o ports.o threads.o interrupts.o keyboard.o

echo "running in qemu"
qemu-system-i386 -kernel kernel.elf -serial stdio

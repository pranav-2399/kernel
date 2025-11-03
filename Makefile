# Cross-compiler
CC     = i386-elf-gcc
LD     = i386-elf-ld
AS     = nasm
CP     = cp
RM     = rm -rf
MKDIR  = mkdir -pv

# Flags
CFLAGS = -m32 -ffreestanding -fno-pic -fno-pie -Wall -Wextra -O0
LDFLAGS = -m elf_i386 -T linker.ld

# Output
BIN      = kernel.elf
ISO      = my-kernel.iso
ISO_PATH = iso
BOOT_PATH = $(ISO_PATH)/boot
GRUB_PATH = $(BOOT_PATH)/grub

# Sources
OBJS = boot.o kernel.o isr.o ports.o threads.o interrupts.o keyboard.o commands.o string.o

.PHONY: all clean run

all: $(ISO)
	@echo "Build complete."

boot.o: boot.asm
	$(AS) -f elf32 boot.asm -o $@

isr.o: isr.asm
	$(AS) -f elf32 isr.asm -o $@

kernel.o: kernel.c
	$(CC) $(CFLAGS) -c kernel.c -o $@

ports.o: ports.c
	$(CC) $(CFLAGS) -c ports.c -o $@

threads.o: threads.c
	$(CC) $(CFLAGS) -c threads.c -o $@

interrupts.o: interrupts.c
	$(CC) $(CFLAGS) -c interrupts.c -o $@

keyboard.o: keyboard.c
	$(CC) $(CFLAGS) -c keyboard.c -o $@

commands.o: commands.c
	$(CC) $(CFLAGS) -c commands.c -o $@

string.o: string.c
	$(CC) $(CFLAGS) -c string.c -o $@

$(BIN): $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $(BIN) $(OBJS)

$(ISO): $(BIN) grub.cfg
	$(MKDIR) $(GRUB_PATH)
	$(CP) $(BIN) $(BOOT_PATH)
	$(CP) grub.cfg $(GRUB_PATH)
	grub-file --is-x86-multiboot $(BOOT_PATH)/$(BIN)
	grub-mkrescue -o $(ISO) $(ISO_PATH)

clean:
	$(RM) *.o $(BIN) $(ISO) $(ISO_PATH)

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)

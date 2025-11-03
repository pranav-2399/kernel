
ifeq ($(shell which i386-elf-gcc 2>/dev/null),)
    CC := x86_64-elf-gcc
    LD := x86_64-elf-ld
    TOOLCHAIN := x86_64-elf
else
    CC := i386-elf-gcc
    LD := i386-elf-ld
    TOOLCHAIN := i386-elf
endif

AS     = nasm
CP     = cp
RM     = rm -rf
MKDIR  = mkdir -pv

CFLAGS  = -m32 -ffreestanding -fno-pic -fno-pie -Wall -Wextra -O0
LDFLAGS = -m elf_i386 -T linker.ld

BIN       = kernel.elf
ISO       = my-kernel.iso
ISO_PATH  = iso
BOOT_PATH = $(ISO_PATH)/boot
GRUB_PATH = $(BOOT_PATH)/grub

OBJS = boot.o isr.o kernel.o ports.o threads.o interrupts.o keyboard.o commands.o string.o

.PHONY: all clean run iso


all: $(BIN)
	@echo "Build complete with toolchain: $(TOOLCHAIN)"

boot.o: boot.asm
	@echo "Assembling $<"
	$(AS) -f elf32 $< -o $@

isr.o: isr.asm
	@echo "Assembling $<"
	$(AS) -f elf32 $< -o $@

%.o: %.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS) linker.ld
	@echo "Linking $@"
	$(LD) $(LDFLAGS) -o $@ $(OBJS)


iso: $(BIN) grub.cfg
	@echo "Building bootable ISO..."
	$(MKDIR) $(GRUB_PATH)
	$(CP) $(BIN) $(BOOT_PATH)
	$(CP) grub.cfg $(GRUB_PATH)
	grub-file --is-x86-multiboot $(BOOT_PATH)/$(BIN)
	grub-mkrescue -o $(ISO) $(ISO_PATH)
	@echo "ISO image created: $(ISO)"


run: $(BIN)
	@echo "Running kernel using QEMU..."
	qemu-system-i386 -kernel $(BIN) -serial stdio

clean:
	@echo "Cleaning build files..."
	$(RM) *.o $(BIN) $(ISO) $(ISO_PATH)

# Tools
CP      := cp
RM      := rm -rf
MKDIR   := mkdir -pv
NASM    := nasm
CC      := i386-elf-gcc
LD      := i386-elf-ld
GRUBMK  := grub-mkrescue
GRUBFILE:= grub-file

# Kernel / ISO names
BIN      := kernel.elf
CFG      := grub.cfg
ISO_PATH := iso
BOOT_PATH:= $(ISO_PATH)/boot
GRUB_PATH:= $(BOOT_PATH)/grub

# Compiler flags
CFLAGS := -m32 -ffreestanding -c
LDFLAGS := -m elf_i386 -T linker.ld

.PHONY: all
all: bootloader kernel linker iso
	@echo "Build completed successfully."

# Build bootloader
bootloader: boot.asm
	$(NASM) -f elf32 boot.asm -o boot.o

# Compile kernel
kernel: kernel.c
	$(CC) $(CFLAGS) kernel.c -o kernel.o

# Link kernel + bootloader
linker: boot.o kernel.o linker.ld
	$(LD) $(LDFLAGS) -o $(BIN) boot.o kernel.o

# Create bootable ISO
iso: $(BIN)
	$(MKDIR) $(GRUB_PATH)
	$(CP) $(BIN) $(BOOT_PATH)/$(BIN)
	$(CP) $(CFG) $(GRUB_PATH)
	$(GRUBFILE) --is-x86-multiboot $(BOOT_PATH)/$(BIN)
	$(GRUBMK) -o my-kernel.iso $(ISO_PATH)

# Clean build artifacts
.PHONY: clean
clean:
	$(RM) *.o $(BIN) *iso $(ISO_PATH)

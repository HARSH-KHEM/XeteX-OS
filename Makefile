CC      = x86_64-elf-gcc
LD      = x86_64-elf-ld
AS      = nasm

CFLAGS  = -ffreestanding -O2 -Wall -Wextra \
          -nostdlib -nostdinc -fno-builtin \
          -fno-stack-protector -m32
ASFLAGS = -f elf32
LDFLAGS = -T boot/linker.ld -nostdlib -m elf_i386

.PHONY: all iso run clean

all: build/xetex.bin

build/xetex.bin: build/boot.o build/kernel.o
	@mkdir -p build
	$(LD) $(LDFLAGS) -o $@ $^
	@echo "Kernel binary built"

build/boot.o: boot/boot.asm
	@mkdir -p build
	$(AS) $(ASFLAGS) $< -o $@

build/kernel.o: kernel/kernel.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

iso: build/xetex.bin
	cp build/xetex.bin iso/boot/
	cp boot/grub.cfg iso/boot/grub/
	x86_64-elf-grub-mkrescue -o build/xetex.iso iso
	@echo "ISO created"

run: iso
	qemu-system-x86_64 -boot d \
		-cdrom build/xetex.iso \
		-m 256M \
		-serial stdio \
		-no-reboot \
		-no-shutdown

clean:
	rm -rf build/

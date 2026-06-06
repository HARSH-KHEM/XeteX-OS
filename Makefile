CC      = x86_64-elf-gcc
LD      = x86_64-elf-ld
AS      = nasm

CFLAGS  = -ffreestanding -O2 -Wall -Wextra -nostdlib -fno-builtin -fno-stack-protector -m32 -I kernel/arch
ASFLAGS = -f elf32
LDFLAGS = -T boot/linker.ld -nostdlib -m elf_i386

OBJS = build/boot.o build/kernel.o \
       build/gdt.o build/gdt_asm.o \
       build/idt.o build/idt_asm.o \
       build/isr.o

.PHONY: all iso run run-iso clean

all: build/xetex.bin

build/xetex.bin: $(OBJS)
	@mkdir -p build
	$(LD) $(LDFLAGS) -o $@ $^
	@echo "Kernel binary built"

build/boot.o: boot/boot.asm
	@mkdir -p build
	$(AS) $(ASFLAGS) $< -o $@

build/kernel.o: kernel/kernel.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/gdt.o: kernel/arch/gdt.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/gdt_asm.o: kernel/arch/gdt.asm
	@mkdir -p build
	$(AS) $(ASFLAGS) $< -o $@

build/idt.o: kernel/arch/idt.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/idt_asm.o: kernel/arch/idt.asm
	@mkdir -p build
	$(AS) $(ASFLAGS) $< -o $@

build/isr.o: kernel/arch/isr.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

# Direct multiboot boot via QEMU -- no ISO or GRUB needed.
run: build/xetex.bin
	qemu-system-i386 -kernel build/xetex.bin -m 256M -serial stdio -no-reboot -no-shutdown

# ISO target -- requires i686-elf-grub (brew install i686-elf-grub)
iso: build/xetex.bin
	@mkdir -p iso/boot/grub
	cp build/xetex.bin iso/boot/
	cp boot/grub.cfg iso/boot/grub/
	i686-elf-grub-mkrescue -o build/xetex.iso iso
	@echo "ISO created"

# Boot via ISO
run-iso: iso
	qemu-system-i386 -boot d -cdrom build/xetex.iso -m 256M -serial stdio -no-reboot -no-shutdown

clean:
	rm -rf build/

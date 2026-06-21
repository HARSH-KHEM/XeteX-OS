CC      = x86_64-elf-gcc
LD      = x86_64-elf-ld
AS      = nasm

CFLAGS  = -ffreestanding -O2 -Wall -Wextra -nostdlib -fno-builtin -fno-stack-protector -m32 -mno-sse -mno-sse2 -mgeneral-regs-only -I kernel/arch -I kernel/drivers -I kernel/lib -I kernel/include -I kernel/memory
ASFLAGS = -f elf32
LDFLAGS = -T boot/linker.ld -nostdlib -m elf_i386

OBJS = build/boot.o \
       build/kernel.o \
       build/gdt.o build/gdt_asm.o \
       build/idt.o build/idt_asm.o \
       build/isr.o \
       build/vga.o \
       build/kprintf.o \
       build/string.o \
       build/pic.o \
       build/pmm.o \
       build/pmmBuddy.o

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

# YOUR NEW FILES
build/vga.o: kernel/drivers/vga.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/kprintf.o: kernel/lib/kprintf.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/string.o: kernel/lib/string.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/pic.o: kernel/arch/pic.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/pmm.o: kernel/memory/pmm.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

build/pmmBuddy.o: kernel/memory/pmmBuddy.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

run: build/xetex.bin
	qemu-system-i386 -kernel build/xetex.bin -m 256M -serial stdio -no-reboot -no-shutdown

iso: build/xetex.bin
	@mkdir -p iso/boot/grub
	cp build/xetex.bin iso/boot/
	cp boot/grub.cfg iso/boot/grub/
	i686-elf-grub-mkrescue -o build/xetex.iso iso
	@echo "ISO created"

run-iso: iso
	qemu-system-i386 -boot d -cdrom build/xetex.iso -m 256M -serial stdio -no-reboot -no-shutdown

clean:
	rm -rf build/
# XeteX-OS

A custom operating system built from scratch in Assembly and C.

## Progress

- [x] Day 1-6: Learned Assembly basics, wrote bootloader, got QEMU running
- [x] Day 7: Cross compiler setup (x86_64-elf-gcc, x86_64-elf-ld), GRUB setup, boots into C kernel with VGA text output

## Build
Requires: nasm, x86_64-elf-gcc, x86_64-elf-ld, Docker, QEMU

```bash
make                  # compile
make iso-docker       # build bootable ISO (uses Docker for BIOS grub)
make run              # launch in QEMU
```

## Collaborators
- Harsh Khem
- Kartik Ojha

MAGIC    equ 0x1BADB002
FLAGS    equ 0x0
CHECKSUM equ -(MAGIC + FLAGS)

section .multiboot
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss
    resb 16384
stack_top:

section .text
bits 32
global _start
extern kernel_main

_start:
    mov esp, stack_top
    call kernel_main
    hlt

global gdt_flush

gdt_flush:
    mov eax, [esp+4]   ; get the gdt_ptr address passed in
    lgdt [eax]         ; load it into the GDTR register

    mov ax, 0x10       ; 0x10 = entry 2 (kernel data), selector index
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; far jump to reload CS with kernel code selector (entry 1 = 0x08)
    jmp 0x08:.flush
.flush:
    ret
global idt_flush
extern isr_handler

; -------------------------------------------------------
; idt_flush -- loads the IDTR register
; -------------------------------------------------------
idt_flush:
    mov eax, [esp+4]   ; get the idt_ptr address passed in
    lidt [eax]         ; load it into the IDTR register
    ret

; -------------------------------------------------------
; ISR stubs for CPU exceptions 0-7
;
; Exceptions that DON'T push an error code: we push a
; dummy 0 so the stack frame is uniform for all ISRs.
; -------------------------------------------------------

; macro for exceptions that don't push an error code
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push dword 0        ; dummy error code
    push dword %1       ; interrupt number
    jmp isr_common_stub
%endmacro

; macro for exceptions that push an error code (not used for 0-7 but ready)
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push dword %1       ; interrupt number (error code already on stack)
    jmp isr_common_stub
%endmacro

; exceptions 0-7 -- none of these push error codes
ISR_NOERRCODE 0   ; divide by zero
ISR_NOERRCODE 1   ; debug
ISR_NOERRCODE 2   ; NMI
ISR_NOERRCODE 3   ; breakpoint
ISR_NOERRCODE 4   ; overflow
ISR_NOERRCODE 5   ; bound range exceeded
ISR_NOERRCODE 6   ; invalid opcode
ISR_NOERRCODE 7   ; device not available

; -------------------------------------------------------
; isr_common_stub -- saves registers, calls C handler
; -------------------------------------------------------
isr_common_stub:
    pusha              ; push edi, esi, ebp, esp, ebx, edx, ecx, eax

    ; save data segment
    mov ax, ds
    push eax

    ; load kernel data segment (0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler   ; call C handler -- registers_t passed on stack

    ; restore data segment
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa               ; pop edi, esi, ebp, esp, ebx, edx, ecx, eax
    add esp, 8         ; pop error code + interrupt number
    sti
    iret               ; return from interrupt

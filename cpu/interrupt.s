; cpu/interrupt.s

; 1. Define external C functions we need to call
[EXTERN isr_handler]
[EXTERN irq_handler]  ; <--- This was likely missing or misplaced

; 2. Define global symbols so C can call us
[GLOBAL isr0]
[GLOBAL irq0]

; --- PART 1: CPU EXCEPTIONS (Crashes) ---

; 0: Divide By Zero Exception
isr0:
    cli
    push byte 0
    push byte 0
    jmp isr_common_stub

; Common ISR stub
isr_common_stub:
    pusha               ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    xor eax, eax        ; Save Data Segment
    mov ax, ds
    push eax

    mov ax, 0x10        ; Load Kernel Data Segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler    ; Call C function (idt.c)

    pop eax             ; Restore Data Segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa                ; Restore Registers
    add esp, 8          ; Clean up stack
    iret                ; Return from interrupt

; --- PART 2: HARDWARE INTERRUPTS (Timer, Keyboard) ---

; 32: IRQ0 (Timer)
irq0:
    cli
    push byte 0
    push byte 32
    jmp irq_common_stub

; Common IRQ stub
irq_common_stub:
    pusha
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    call irq_handler    ; Call C function (idt.c)
    
    pop eax 
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa
    add esp, 8
    iret

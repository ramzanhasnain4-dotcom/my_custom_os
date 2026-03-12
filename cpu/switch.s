[GLOBAL switch_task]

; void switch_task(uint32_t *old_esp_ptr, uint32_t new_esp);
switch_task:
    ; 1. Save all registers
    push ebp
    push ebx
    push esi
    push edi
    
    ; 2. SAVE FLAGS (This saves the Interrupt State)
    pushf

    ; 3. Load Arguments (Offsets adjusted for the 5 pushes above)
    mov eax, [esp + 24]   ; Argument 1: &old_esp
    mov edx, [esp + 28]   ; Argument 2: new_esp

    ; 4. Perform the Switch
    mov [eax], esp        ; Save current ESP
    mov esp, edx          ; Load new ESP

    ; 5. Restore registers
    popf                  ; RESTORE FLAGS (This turns Interrupts ON for the new task)
    pop edi
    pop esi
    pop ebx
    pop ebp

    ret

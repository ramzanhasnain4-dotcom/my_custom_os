#include "idt.h"
#include "ports.h"

extern void isr0();
extern void irq0();
extern void timer_callback(); // From timer.c

idt_gate_t idt[256];
idt_register_t idt_reg;

void set_idt_gate(int n, uint32_t handler) {
    idt[n].low_offset = (uint16_t)((handler) & 0xFFFF);
    idt[n].sel = 0x08;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E; 
    idt[n].high_offset = (uint16_t)(((handler) >> 16) & 0xFFFF);
}

void load_idt() {
    idt_reg.base = (uint32_t) &idt;
    idt_reg.limit = 256 * sizeof(idt_gate_t) - 1;
    __asm__ __volatile__("lidt (%0)" : : "r" (&idt_reg));
}

void init_idt() {
    // 1. Remap PIC
    port_byte_out(0x20, 0x11);
    port_byte_out(0xA0, 0x11);
    port_byte_out(0x21, 0x20);
    port_byte_out(0xA1, 0x28);
    port_byte_out(0x21, 0x04);
    port_byte_out(0xA1, 0x02);
    port_byte_out(0x21, 0x01);
    port_byte_out(0xA1, 0x01);
    port_byte_out(0x21, 0x0);
    port_byte_out(0xA1, 0x0);

    // 2. Unmask
    port_byte_out(0x21, 0x00);
    port_byte_out(0xA1, 0x00);

    // 3. Install Handlers
    set_idt_gate(0, (uint32_t)isr0);
    set_idt_gate(32, (uint32_t)irq0);

    load_idt();
}

void isr_handler() {
    volatile uint16_t* vga = (uint16_t*)0xB8000;
    vga[0] = (uint16_t)'!' | (uint16_t)0x4F00; 
    while(1);
}

// THIS IS THE FIX
void irq_handler(void *regs) {
    // 1. Send EOI (End of Interrupt) NOW.
    // We must tell the hardware we are done BEFORE we switch stacks.
    // If we don't, the hardware waits forever and never interrupts Task B.
    port_byte_out(0x20, 0x20); 

    // 2. NOW we can safely run the timer logic (which might switch tasks)
    timer_callback(); 
    
    (void)regs;
}

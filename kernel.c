#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "cpu/timer.h"
#include "cpu/scheduler.h"

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;

void kmain() {
    // 1. Print Debug Message FIRST
    const char* debug = "Kernel Started... Loading GDT/IDT";
    int j = 0;
    while(debug[j] != '\0') {
        vga_buffer[j] = (uint16_t)debug[j] | (uint16_t)0x0700;
        j++;
    }

    init_gdt();
    init_idt();

    // 2. Setup Scheduler
    init_scheduler();

    // 3. Start Timer
    init_timer(50);

    // 4. Enable Interrupts
    __asm__ volatile("sti");

    // 5. Start!
    start_multitasking_test();

    while(1);
}

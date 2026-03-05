#include "timer.h"
#include "ports.h"
#include "idt.h"
#include "scheduler.h" 

uint32_t tick = 0;

void timer_callback() {
    tick++;
    
    // CALL SCHEDULER EVERY TICK
    // The scheduler will decide if it's time to switch
    schedule(); 
}

void init_timer(uint32_t freq) {
    uint32_t divisor = 1193180 / freq;
    uint8_t low  = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
    port_byte_out(0x43, 0x36); 
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}

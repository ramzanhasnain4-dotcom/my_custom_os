#include <stdint.h>

unsigned char port_byte_in(uint16_t port) {
    unsigned char result;
    __asm__("inb %1, %0" : "=a" (result) : "dN" (port));
    return result;
}

void port_byte_out(uint16_t port, uint8_t data) {
    __asm__("outb %1, %0" : : "dN" (port), "a" (data));
}

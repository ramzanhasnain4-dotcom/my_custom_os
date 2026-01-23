C_SOURCES = $(wildcard kernel.c cpu/*.c)
HEADERS = $(wildcard cpu/*.h)

# Object files
C_OBJ = ${C_SOURCES:.c=.o}
# Ensure switch.o is included!
ASM_OBJ = cpu/gdt_s.o cpu/interrupt_s.o cpu/switch.o

CC = gcc
# CHANGED: -O2 -> -O0 (Disable optimization)
CFLAGS = -g -m32 -ffreestanding -fno-pie -O0 -Wall -Wextra

myos.bin: boot.o ${C_OBJ} ${ASM_OBJ} linker.ld
	${CC} -m32 -no-pie -T linker.ld -o myos.bin -ffreestanding -O0 -nostdlib boot.o ${C_OBJ} ${ASM_OBJ} -lgcc

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -c $< -o $@

boot.o: boot.s
	nasm -felf32 boot.s -o boot.o

cpu/gdt_s.o: cpu/gdt.s
	nasm -felf32 cpu/gdt.s -o cpu/gdt_s.o

cpu/interrupt_s.o: cpu/interrupt.s
	nasm -felf32 cpu/interrupt.s -o cpu/interrupt_s.o

cpu/switch.o: cpu/switch.s
	nasm -felf32 cpu/switch.s -o cpu/switch.o

run: myos.bin
	qemu-system-i386 -kernel myos.bin

clean:
	rm -f *.o *.bin cpu/*.o
# "fresh" cleans everything and then runs
fresh:
	rm -f *.o cpu/*.o myos.bin
	make run

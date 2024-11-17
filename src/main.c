#include "cpu.h"
#include "memory.h"
#include <stdio.h>

#define MEMORY_SIZE 65536

int main(int argc, char**argv) {
    if (argc < 2) {
        printf("Usage: <program> <bin_file>\n");
        return 1;
    }

    CPU cpu;
    initialize_memory();
    reset_cpu(&cpu);
    
    const uint16_t load_address = 0x0600;
    load_program(argv[1], load_address);

    while (cpu.is_running) {
        execute(&cpu, memory);
    }

    printf("Final CPU State:\n");
    printf("Accumulator: %02X\n", cpu.A);
    printf("X Register: %02X\n", cpu.X);
    printf("Y Register: %02X\n", cpu.Y);
    printf("Status: %02X\n", cpu.status);
    printf("Program Counter: %04X\n", cpu.PC);
    printf("Stack Pointer: %02X\n", cpu.SP);

    return 0;
}

#include "cpu.h"
#include <stdio.h>

#define MEMORY_SIZE 65536

int main() {
    CPU cpu;
    uint8_t memory[MEMORY_SIZE] = {0};

    reset_cpu(&cpu);

    // Load a sample program into memory
    // Program starts at 0x600
    memory[0x600] = 0xA9; // LDA #$0A
    memory[0x601] = 0x0A;
    memory[0x602] = 0x8D; // STA $8000
    memory[0x603] = 0x00;
    memory[0x604] = 0x80;
    memory[0x605] = 0xE8; // INX
    memory[0x606] = 0xAA; // TAX
    memory[0x607] = 0xCA; // DEX
    memory[0x608] = 0x69; // ADC #$05
    memory[0x609] = 0x05;
    memory[0x60A] = 0x48; // PHA
    memory[0x60B] = 0x68; // PLA
    memory[0x60C] = 0x20; // JSR $060F
    memory[0x60D] = 0x0F;
    memory[0x60E] = 0x06;
    memory[0x60F] = 0xE8; // INX (subroutine)
    memory[0x610] = 0x60; // RTS
    memory[0x611] = 0xF0; // BEQ (Branch if zero)
    memory[0x612] = 0x02;
    memory[0x613] = 0xA9; // LDA #$00
    memory[0x614] = 0x00;
    memory[0x615] = 0xD0; // BNE (Branch if not equal)
    memory[0x616] = 0xFE;

    // Execute the program
    while (cpu.PC < 0x617) {
        execute(&cpu, memory);
    }

    // Print out some results for verification
    printf("Accumulator: %02X\n", cpu.A);
    printf("X Register: %02X\n", cpu.X);
    printf("Y Register: %02X\n", cpu.Y);
    printf("Status: %02X\n", cpu.status);
    printf("Memory at $8000: %02X\n", memory[0x8000]);

    return 0;
}

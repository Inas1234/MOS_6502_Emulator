#include "memory.h"

// Global memory arrays
uint8_t memory[MEMORY_SIZE] = {0};
uint8_t rom[ROM_SIZE] = {0};

void initialize_memory() {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }
}

uint8_t read_memory(uint16_t address) {
    if (address <= ZERO_PAGE_END) {
        return memory[address];
    } else if (address >= STACK_START && address <= STACK_END) {
        return memory[address];
    } else if (address >= RAM_START && address <= RAM_END) { 
        return memory[address];
    } else if (address >= MIRRORED_RAM_START && address <= MIRRORED_RAM_END) {
        return memory[address % 0x0800];
    } else if (address >= IO_REGISTERS_START && address <= IO_REGISTERS_END) { 
        return handle_io_read(address);
    } else if (address >= ROM_START && address <= ROM_END) { 
        return rom[address - ROM_START];
    }
    return 0xFF;
}

// Write to memory
void write_memory(uint16_t address, uint8_t value) {
    if (address <= ZERO_PAGE_END) { 
        memory[address] = value;
    } else if (address >= STACK_START && address <= STACK_END) { 
        memory[address] = value;
    } else if (address >= RAM_START && address <= RAM_END) { 
        memory[address] = value;
    } else if (address >= MIRRORED_RAM_START && address <= MIRRORED_RAM_END) { 
        memory[address % 0x0800] = value;
    } else if (address >= IO_REGISTERS_START && address <= IO_REGISTERS_END) { 
        handle_io_write(address, value);
    } else if (address >= ROM_START && address <= ROM_END) { // ROM is read-only
        printf("Warning: Attempt to write to ROM at address $%04X\n", address);
    }
}

void load_rom(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error: Unable to open ROM file %s\n", filename);
        return;
    }

    fread(rom, sizeof(uint8_t), ROM_SIZE, file);
    fclose(file);
    printf("Loaded ROM: %s\n", filename);
}

// Placeholder for I/O read (to be implemented)
uint8_t handle_io_read(uint16_t address) {
    printf("Reading from I/O register $%04X\n", address);
    return 0x00;
}

// Placeholder for I/O write (to be implemented)
void handle_io_write(uint16_t address, uint8_t value) {
    printf("Writing value $%02X to I/O register $%04X\n", value, address);
}

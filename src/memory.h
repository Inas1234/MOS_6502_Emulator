#ifndef MEMORY_H
#define MEMORY_H
#include "../include/common.h"

extern uint8_t memory[MEMORY_SIZE];
extern uint8_t rom[ROM_SIZE];

// Function prototypes
void initialize_memory();
uint8_t read_memory(uint16_t address);
void write_memory(uint16_t address, uint8_t value);
void load_rom(const char *filename);
uint8_t handle_io_read(uint16_t address);
void handle_io_write(uint16_t address, uint8_t value);

#endif
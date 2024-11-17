#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 65536
#define ROM_SIZE 32768   // 32 KB ROM
#define ZERO_PAGE_END        0x00FF
#define STACK_START          0x0100
#define STACK_END            0x01FF
#define RAM_START            0x0200
#define RAM_END              0x07FF
#define MIRRORED_RAM_START   0x0800
#define MIRRORED_RAM_END     0x1FFF
#define IO_REGISTERS_START   0x2000
#define IO_REGISTERS_END     0x3FFF
#define ROM_START            0x8000
#define ROM_END              0xFFFF

#define SET_FLAG(cpu, flag)   ( cpu->status |= flag )
#define CLEAR_FLAG(cpu, flag) ( cpu->status &= ~flag )
#define CHECK_FLAG(cpu, flag) ( cpu->status & flag )


#endif
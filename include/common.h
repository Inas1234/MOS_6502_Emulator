#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 65536
#define SET_FLAG(cpu, flag) (cpu->status |= (flag))
#define CLEAR_FLAG(cpu, flag) (cpu->status &= ~(flag))
#define CHECK_FLAG(cpu, flag) (cpu->status & (flag))



#endif
#ifndef CPU_H
#define CPU_H

#include "../include/common.h"

#define FLAG_NEGATIVE     0x80
#define FLAG_OVERFLOW     0x40
#define FLAG_UNUSED       0x20
#define FLAG_BREAK        0x10
#define FLAG_DECIMAL      0x08
#define FLAG_INTERRUPT    0x04
#define FLAG_ZERO         0x02
#define FLAG_CARRY        0x01


typedef struct {
    uint8_t A;        
    uint8_t X;       
    uint8_t Y;        
    uint8_t SP;      
    uint8_t status;   
    uint16_t PC;     
} CPU;

void reset_cpu(CPU * cpu);
uint8_t fetch(CPU * cpu, uint8_t * memory);
void execute(CPU *cpu, uint8_t * memory);


void lda_immediate(CPU *cpu, uint8_t *memory);
void lda_zero_page(CPU *cpu, uint8_t *memory);
void lda_absolute(CPU *cpu, uint8_t *memory);
void ldx_immediate(CPU *cpu, uint8_t *memory);
void ldy_immediate(CPU *cpu, uint8_t *memory);
void sta_absolute(CPU *cpu, uint8_t *memory);
void stx_zero_page(CPU *cpu, uint8_t *memory);
void sty_zero_page(CPU *cpu, uint8_t *memory);

// Arithmetic Operations
void adc_immediate(CPU *cpu, uint8_t *memory);
void sbc_immediate(CPU *cpu, uint8_t *memory);
void cmp_immediate(CPU *cpu, uint8_t *memory);
void cpx_immediate(CPU *cpu, uint8_t *memory);
void cpy_immediate(CPU *cpu, uint8_t *memory);
void inc_zero_page(CPU *cpu, uint8_t *memory);
void dec_zero_page(CPU *cpu, uint8_t *memory);
void inx(CPU *cpu);
void iny(CPU *cpu);
void dex(CPU *cpu);
void dey(CPU *cpu);

// Bitwise Operations
void and_immediate(CPU *cpu, uint8_t *memory);
void eor_immediate(CPU *cpu, uint8_t *memory);
void ora_immediate(CPU *cpu, uint8_t *memory);
void asl_accumulator(CPU *cpu);
void lsr_accumulator(CPU *cpu);
void rol_accumulator(CPU *cpu);
void ror_accumulator(CPU *cpu);

// Branch Operations
void bcc(CPU *cpu, uint8_t *memory);
void bcs(CPU *cpu, uint8_t *memory);
void beq(CPU *cpu, uint8_t *memory);
void bmi(CPU *cpu, uint8_t *memory);
void bne(CPU *cpu, uint8_t *memory);
void bpl(CPU *cpu, uint8_t *memory);
void bvc(CPU *cpu, uint8_t *memory);
void bvs(CPU *cpu, uint8_t *memory);

// Jump/Call Operations
void jmp_absolute(CPU *cpu, uint8_t *memory);
void jsr_absolute(CPU *cpu, uint8_t *memory);
void rts(CPU *cpu, uint8_t *memory);
void rti(CPU *cpu, uint8_t *memory);

// Stack Operations
void pha(CPU *cpu, uint8_t *memory);
void php(CPU *cpu, uint8_t *memory);
void pla(CPU *cpu, uint8_t *memory);
void plp(CPU *cpu, uint8_t *memory);

// System Operations
void brk(CPU *cpu);
void nop(CPU *cpu);
void clc(CPU *cpu);
void cld(CPU *cpu);
void cli(CPU *cpu);
void clv(CPU *cpu);
void sec(CPU *cpu);
void sed(CPU *cpu);
void sei(CPU *cpu);

// Register Transfers
void tax(CPU *cpu);
void tay(CPU *cpu);
void txa(CPU *cpu);
void tya(CPU *cpu);

// Decrement Operations
void dec_absolute(CPU *cpu, uint8_t *memory);
void dex(CPU *cpu);
void dey(CPU *cpu);

void bit_zero_page(CPU *cpu, uint8_t *memory);
void bit_absolute(CPU *cpu, uint8_t *memory);

// Increment and Decrement Instructions
void dec_absolute(CPU *cpu, uint8_t *memory);
void dec_zero_page(CPU *cpu, uint8_t *memory);
void inc_absolute(CPU *cpu, uint8_t *memory);
void inc_zero_page(CPU *cpu, uint8_t *memory);

// Jump Instructions
void jmp_indirect(CPU *cpu, uint8_t *memory);

// Shifts and Rotates for Memory
void asl_absolute(CPU *cpu, uint8_t *memory);
void lsr_absolute(CPU *cpu, uint8_t *memory);
void rol_absolute(CPU *cpu, uint8_t *memory);
void ror_absolute(CPU *cpu, uint8_t *memory);

// Additional Addressing Modes
void lda_zero_page_x(CPU *cpu, uint8_t *memory);
void lda_absolute_x(CPU *cpu, uint8_t *memory);
void lda_absolute_y(CPU *cpu, uint8_t *memory);
void lda_indexed_indirect(CPU *cpu, uint8_t *memory);
void lda_indirect_indexed(CPU *cpu, uint8_t *memory);

void sta_zero_page_x(CPU *cpu, uint8_t *memory);
void sta_absolute_x(CPU *cpu, uint8_t *memory);
void sta_absolute_y(CPU *cpu, uint8_t *memory);


#endif
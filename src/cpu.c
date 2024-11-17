#include "cpu.h"
#include "memory.h"

// void update_zero_and_negative_flags(CPU *cpu, uint8_t value) {
//     if (value == 0)
//         cpu->status |= FLAG_ZERO; // Set Zero flag if value is zero
//     else
//         cpu->status &= ~FLAG_ZERO; // Clear Zero flag if non-zero

//     if (value & 0x80)
//         cpu->status |= FLAG_NEGATIVE; // Set Negative flag if high bit is set
//     else
//         cpu->status &= ~FLAG_NEGATIVE; // Clear Negative flag if high bit is not set
// }



void update_zero_and_negative_flags(CPU *cpu, uint8_t value) {
    if (value == 0)
        SET_FLAG(cpu, FLAG_ZERO); // Set Zero flag if value is zero
    else
        CLEAR_FLAG(cpu, FLAG_ZERO); // Clear Zero flag if non-zero

    if (value & 0x80)
        SET_FLAG(cpu, FLAG_NEGATIVE); // Set Negative flag if high bit is set
    else
        CLEAR_FLAG(cpu, FLAG_NEGATIVE); // Clear Negative flag if high bit is not set
    
    CLEAR_FLAG(cpu, FLAG_BREAK);
}


void reset_cpu(CPU * cpu) {
    cpu->A = 0;
    cpu->X = 0;
    cpu->Y = 0;
    cpu->SP = 0xFF;
    cpu->status = 0;
    cpu->PC = 0x600;
    cpu->is_running = 1;
}

void load_program(const char *filename, uint16_t load_address) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error: Unable to open ROM file %s\n", filename);
        return;
    }

    fread(&memory[load_address], sizeof(uint8_t), MEMORY_SIZE - load_address, file);
    fclose(file);
    printf("Loaded program into memory at $%04X\n", load_address);
}


uint8_t fetch(CPU * cpu, uint8_t * memory) {
    (void)memory;
    uint8_t value = read_memory(cpu->PC);
    cpu->PC++;
    return value;
}

void execute(CPU *cpu, uint8_t *memory) {
    uint8_t opcode = fetch(cpu, memory);

    switch (opcode) {
        // Load/Store Instructions
        case 0xA9: lda_immediate(cpu, memory); break;
        case 0xAD: lda_absolute(cpu, memory); break;
        case 0x8D: sta_absolute(cpu, memory); break;
        case 0xA2: ldx_immediate(cpu, memory); break;
        case 0xA0: ldy_immediate(cpu, memory); break;
        case 0x86: stx_zero_page(cpu, memory); break;
        case 0x84: sty_zero_page(cpu, memory); break;

        // Arithmetic Instructions
        case 0x69: adc_immediate(cpu, memory); break;
        case 0xE9: sbc_immediate(cpu, memory); break;
        case 0xC9: cmp_immediate(cpu, memory); break;
        case 0xE0: cpx_immediate(cpu, memory); break;
        case 0xC0: cpy_immediate(cpu, memory); break;
        case 0xE6: inc_zero_page(cpu, memory); break;
        case 0xC6: dec_zero_page(cpu, memory); break;
        case 0xE8: inx(cpu); break;
        case 0xC8: iny(cpu); break;
        case 0xCA: dex(cpu); break;
        case 0x88: dey(cpu); break;

        // Bitwise Operations
        case 0x29: and_immediate(cpu, memory); break;
        case 0x49: eor_immediate(cpu, memory); break;
        case 0x09: ora_immediate(cpu, memory); break;
        case 0x0A: asl_accumulator(cpu); break;
        case 0x4A: lsr_accumulator(cpu); break;
        case 0x2A: rol_accumulator(cpu); break;
        case 0x6A: ror_accumulator(cpu); break;

        // Branch Instructions
        case 0x90: bcc(cpu, memory); break;
        case 0xB0: bcs(cpu, memory); break;
        case 0xF0: beq(cpu, memory); break;
        case 0xD0: bne(cpu, memory); break;
        case 0x30: bmi(cpu, memory); break;
        case 0x10: bpl(cpu, memory); break;
        case 0x50: bvc(cpu, memory); break;
        case 0x70: bvs(cpu, memory); break;

        // Stack Operations
        case 0x48: pha(cpu, memory); break;
        case 0x08: php(cpu, memory); break;
        case 0x68: pla(cpu, memory); break;
        case 0x28: plp(cpu, memory); break;

        // System Operations
        case 0x00: brk(cpu, memory); break;
        case 0x80: case 0x82: case 0x89: case 0xC2: case 0xE2: nop(cpu); break; // NOP Variants

        case 0xA7: case 0xB7: case 0xAF: case 0xBF: case 0xA3: case 0xB3: lax(cpu, memory); break;
        case 0x87: case 0x97: case 0x8F: case 0x83: sax(cpu, memory); break;
        case 0xC7: case 0xD7: case 0xCF: case 0xDF: case 0xDB: case 0xC3: case 0xD3: dcp(cpu, memory); break;
        case 0xE7: case 0xF7: case 0xEF: case 0xFF: case 0xFB: case 0xE3: case 0xF3: isb(cpu, memory); break;
        case 0x07: case 0x17: case 0x0F: case 0x1F: case 0x1B: case 0x03: case 0x13: slo(cpu, memory); break;
        case 0x47: case 0x57: case 0x4F: case 0x5F: case 0x5B: case 0x43: case 0x53: sre(cpu, memory); break;
        case 0x18: clc(cpu); break;
        case 0xD8: cld(cpu); break;
        case 0x58: cli(cpu); break;
        case 0xB8: clv(cpu); break;
        case 0x38: sec(cpu); break;
        case 0xF8: sed(cpu); break;
        case 0x78: sei(cpu); break;

        // Register Transfers
        case 0xAA: tax(cpu); break;
        case 0xA8: tay(cpu); break;
        case 0x8A: txa(cpu); break;
        case 0x98: tya(cpu); break;

        case 0xCE: dec_absolute(cpu, memory); break;

        case 0x4C: jmp_absolute(cpu, memory); break;
        case 0x20: jsr_absolute(cpu, memory); break;
        case 0x60: rts(cpu, memory); break;
        case 0x40: rti(cpu, memory); break;

        case 0x24: bit_zero_page(cpu, memory); break;
        case 0x2C: bit_absolute(cpu, memory); break;
        case 0xEE: inc_absolute(cpu, memory); break;
        case 0x6C: jmp_indirect(cpu, memory); break;
        case 0x0E: asl_absolute(cpu, memory); break;
        case 0x4E: lsr_absolute(cpu, memory); break;
        case 0x2E: rol_absolute(cpu, memory); break;
        case 0x6E: ror_absolute(cpu, memory); break;

        case 0x41: eor_indexed_indirect(cpu, memory); break;   // EOR ($nn, X)
        case 0x71: eor_indirect_indexed(cpu, memory); break;   // EOR ($nn), Y
        case 0x7F: ora_absolute_x(cpu, memory); break;         // ORA $nnnn, X
        case 0x19: ora_absolute_y(cpu, memory); break;         // ORA $nnnn, Y
        case 0x1A: nop(cpu); break;                            // NOP (65C02)
        case 0x04: nop_zero_page(cpu, memory); break;          // NOP $nn (Zero Page)
        case 0x05: ora_zero_page(cpu, memory); break;          // ORA $nn
        case 0x06: asl_zero_page(cpu, memory); break;          // ASL $nn
        case 0x0B: anc_immediate(cpu, memory); break;          // ANC (Unofficial)
        case 0x52: eor_indirect(cpu, memory); break;           // EOR ($nn)
        case 0x54: nop_zero_page_x(cpu, memory); break;        // NOP $nn, X
        case 0x55: eor_zero_page_x(cpu, memory); break;        // EOR $nn, X
        case 0x4B: alr_immediate(cpu, memory); break;          // ALR (Unofficial)
        case 0xFE: inc_absolute_x(cpu, memory); break;


        case 0x02: brk(cpu, memory); break; 

        default:
            printf("Unknown opcode: 0x%02X\n", opcode);
            break;
    }
}


void lda_immediate(CPU *cpu, uint8_t *memory) {
    cpu->A = read_memory(cpu->PC++); 
    update_zero_and_negative_flags(cpu, cpu->A);
}

void lda_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    
    cpu->A = read_memory(address);
    
    update_zero_and_negative_flags(cpu, cpu->A);
}


void sta_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    write_memory(address, cpu->A); 
}


void adc_immediate(CPU *cpu, uint8_t *memory) {
    uint8_t value = fetch(cpu, memory);
    uint16_t result = cpu->A + value + (CHECK_FLAG(cpu, FLAG_CARRY) ? 1 : 0);

    if (result > 0xFF) {
        SET_FLAG(cpu, FLAG_CARRY);
    } else {
        CLEAR_FLAG(cpu, FLAG_CARRY);
    }

    update_zero_and_negative_flags(cpu, result & 0xFF);

    if (((cpu->A ^ value) & 0x80) == 0 && ((cpu->A ^ result) & 0x80)) {
        SET_FLAG(cpu, FLAG_OVERFLOW);
    } else {
        CLEAR_FLAG(cpu, FLAG_OVERFLOW);
    }

    cpu->A = result & 0xFF;
}

void sbc_immediate(CPU *cpu, uint8_t *memory) {
    uint8_t value = fetch(cpu, memory);
    uint16_t result = cpu->A - value - (CHECK_FLAG(cpu, FLAG_CARRY) ? 0 : 1);

    if (result <= 0xFF) {
        SET_FLAG(cpu, FLAG_CARRY);
    } else {
        CLEAR_FLAG(cpu, FLAG_CARRY);
    }

    update_zero_and_negative_flags(cpu, result & 0xFF);

    if (((cpu->A ^ value) & 0x80) && ((cpu->A ^ result) & 0x80)) {
        SET_FLAG(cpu, FLAG_OVERFLOW);
    } else {
        CLEAR_FLAG(cpu, FLAG_OVERFLOW);
    }

    cpu->A = result & 0xFF;
}

void ldx_immediate(CPU *cpu, uint8_t *memory) {
    cpu->X = fetch(cpu, memory);
    update_zero_and_negative_flags(cpu, cpu->X);
}

void ldy_immediate(CPU *cpu, uint8_t *memory) {
    cpu->Y = fetch(cpu, memory);
    update_zero_and_negative_flags(cpu, cpu->Y);
}

void stx_zero_page(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory);
    write_memory(address, cpu->X); // Write using write_memory
}

void sty_zero_page(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory);
    write_memory(address, cpu->Y); // Write using write_memory
}

void cmp_immediate(CPU *cpu, uint8_t *memory) {
    uint8_t value = fetch(cpu, memory);
    uint16_t result = cpu->A - value;
    
    if (cpu->A >= value) {
        SET_FLAG(cpu, FLAG_CARRY);
    } else {
        CLEAR_FLAG(cpu, FLAG_CARRY);
    }

    update_zero_and_negative_flags(cpu, result & 0xFF);
}

void cpx_immediate(CPU *cpu, uint8_t *memory) {
    uint8_t value = fetch(cpu, memory);
    uint16_t result = cpu->X - value;

    if (cpu->X >= value)
        SET_FLAG(cpu, FLAG_CARRY);
    else
        CLEAR_FLAG(cpu, FLAG_CARRY);

    update_zero_and_negative_flags(cpu, (uint8_t)result);
}

void cpy_immediate(CPU *cpu, uint8_t *memory) {
    uint8_t value = fetch(cpu, memory);
    uint16_t result = cpu->Y - value;

    if (cpu->Y >= value)
        SET_FLAG(cpu, FLAG_CARRY);
    else
        CLEAR_FLAG(cpu, FLAG_CARRY);

    update_zero_and_negative_flags(cpu, (uint8_t)result);
}

void inx(CPU *cpu) {
    cpu->X++;
    update_zero_and_negative_flags(cpu, cpu->X);
}

void iny(CPU *cpu) {
    cpu->Y++;
    update_zero_and_negative_flags(cpu, cpu->Y);
}

void dex(CPU *cpu) {
    cpu->X--;
    update_zero_and_negative_flags(cpu, cpu->X);
}

void dey(CPU *cpu) {
    cpu->Y--;
    update_zero_and_negative_flags(cpu, cpu->Y);
}

void and_immediate(CPU *cpu, uint8_t *memory) {
    cpu->A &= fetch(cpu, memory);
    update_zero_and_negative_flags(cpu, cpu->A);
}

void eor_immediate(CPU *cpu, uint8_t *memory) {
    cpu->A ^= fetch(cpu, memory);
    update_zero_and_negative_flags(cpu, cpu->A);
}

void ora_immediate(CPU *cpu, uint8_t *memory) {
    cpu->A |= fetch(cpu, memory);
    update_zero_and_negative_flags(cpu, cpu->A);
}

void asl_accumulator(CPU *cpu) {
    uint8_t result = cpu->A << 1;
    
    if (cpu->A & 0x80)
        SET_FLAG(cpu, FLAG_CARRY);
    else
        CLEAR_FLAG(cpu, FLAG_CARRY);

    cpu->A = result;
    update_zero_and_negative_flags(cpu, cpu->A);
}

void lsr_accumulator(CPU *cpu) {
    if (cpu->A & 0x01)
        SET_FLAG(cpu, FLAG_CARRY);
    else
        CLEAR_FLAG(cpu, FLAG_CARRY);

    cpu->A >>= 1;
    update_zero_and_negative_flags(cpu, cpu->A);
}

void rol_accumulator(CPU *cpu) {
    uint8_t carry_in = CHECK_FLAG(cpu, FLAG_CARRY);
    if (cpu->A & 0x80)
        SET_FLAG(cpu, FLAG_CARRY);
    else
        CLEAR_FLAG(cpu, FLAG_CARRY);

    cpu->A = (cpu->A << 1) | carry_in;
    update_zero_and_negative_flags(cpu, cpu->A);
}

void ror_accumulator(CPU *cpu) {
    uint8_t carry_in = CHECK_FLAG(cpu, FLAG_CARRY) ? 0x80 : 0x00;
    if (cpu->A & 0x01)
        SET_FLAG(cpu, FLAG_CARRY);
    else
        CLEAR_FLAG(cpu, FLAG_CARRY);

    cpu->A = (cpu->A >> 1) | carry_in;
    update_zero_and_negative_flags(cpu, cpu->A);
}

void bcc(CPU *cpu, uint8_t *memory) {
    int8_t offset = fetch(cpu, memory);
    if (!CHECK_FLAG(cpu, FLAG_CARRY)) cpu->PC += offset;
}

void bcs(CPU *cpu, uint8_t *memory) {
    int8_t offset = fetch(cpu, memory);
    if (CHECK_FLAG(cpu, FLAG_CARRY)) cpu->PC += offset;
}

void beq(CPU *cpu, uint8_t *memory) {
    int8_t offset = fetch(cpu, memory);
    if (CHECK_FLAG(cpu, FLAG_ZERO)) cpu->PC += offset;
}

void bne(CPU *cpu, uint8_t *memory) {
    int8_t offset = fetch(cpu, memory);
    if (!CHECK_FLAG(cpu, FLAG_ZERO)) cpu->PC += offset;
}

void pha(CPU *cpu, uint8_t *memory) {
    write_memory(0x0100 + cpu->SP--, cpu->A); // Use write_memory for stack push
}

void php(CPU *cpu, uint8_t *memory) {
    write_memory(0x0100 + cpu->SP--, cpu->status | FLAG_BREAK | FLAG_UNUSED);
}

void pla(CPU *cpu, uint8_t *memory) {
    cpu->A = read_memory(0x0100 + ++cpu->SP); // Use read_memory for stack pop
    update_zero_and_negative_flags(cpu, cpu->A);
}

void plp(CPU *cpu, uint8_t *memory) {
    cpu->status = read_memory(0x0100 + ++cpu->SP) & ~FLAG_UNUSED;
}

void nop(CPU *cpu) {
    (void)cpu; // No operation, just advance the program counter
}

void lax(CPU *cpu, uint8_t *memory) {
    uint8_t value = fetch(cpu, memory);
    cpu->A = value;
    cpu->X = value;
    update_zero_and_negative_flags(cpu, value);
}

void sax(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory);
    write_memory(address, cpu->A & cpu->X); // Use write_memory
}

void dcp(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory);
    uint8_t value = read_memory(address) - 1; // Decrement using read_memory
    write_memory(address, value); // Use write_memory
    if (cpu->A >= value) {
        SET_FLAG(cpu, FLAG_CARRY);
    } else {
        CLEAR_FLAG(cpu, FLAG_CARRY);
    }
    update_zero_and_negative_flags(cpu, cpu->A - value);
}

void isb(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory);
    uint8_t value = read_memory(address) + 1; // Increment using read_memory
    write_memory(address, value); // Use write_memory
    uint16_t result = cpu->A - value - (CHECK_FLAG(cpu, FLAG_CARRY) ? 0 : 1);
    update_zero_and_negative_flags(cpu, result & 0xFF);
    cpu->A = result & 0xFF;
}

void slo(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory);
    uint8_t value = read_memory(address) << 1; // Shift using read_memory
    write_memory(address, value); // Use write_memory
    cpu->A |= value;
    update_zero_and_negative_flags(cpu, cpu->A);
}

void sre(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory);
    uint8_t value = read_memory(address) >> 1; // Shift using read_memory
    write_memory(address, value); // Use write_memory
    cpu->A ^= value;
    update_zero_and_negative_flags(cpu, cpu->A);
}

void clc(CPU *cpu) {
    CLEAR_FLAG(cpu, FLAG_CARRY);
}

void cld(CPU *cpu) {
    CLEAR_FLAG(cpu, FLAG_DECIMAL);
}

void cli(CPU *cpu) {
    CLEAR_FLAG(cpu, FLAG_INTERRUPT);
}

void clv(CPU *cpu) {
    CLEAR_FLAG(cpu, FLAG_OVERFLOW);
}

void sec(CPU *cpu) {
    SET_FLAG(cpu, FLAG_CARRY);
}

void sed(CPU *cpu) {
    SET_FLAG(cpu, FLAG_DECIMAL);
}

void sei(CPU *cpu) {
    SET_FLAG(cpu, FLAG_INTERRUPT);
}


void tax(CPU *cpu) {
    cpu->X = cpu->A;
    update_zero_and_negative_flags(cpu, cpu->X);
}

void tay(CPU *cpu) {
    cpu->Y = cpu->A;
    update_zero_and_negative_flags(cpu, cpu->Y);
}

void txa(CPU *cpu) {
    cpu->A = cpu->X;
    update_zero_and_negative_flags(cpu, cpu->A);
}

void tya(CPU *cpu) {
    cpu->A = cpu->Y;
    update_zero_and_negative_flags(cpu, cpu->A);
}

void dec_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    uint8_t value = read_memory(address); // Read value using read_memory
    value--;
    write_memory(address, value); // Write back using write_memory
    update_zero_and_negative_flags(cpu, value);
}


void jmp_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    cpu->PC = address;
}

void jsr_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    
    uint16_t return_address = cpu->PC - 1;
    write_memory(0x0100 + cpu->SP--, (return_address >> 8) & 0xFF); // High byte
    write_memory(0x0100 + cpu->SP--, return_address & 0xFF); // Low byte
    
    cpu->PC = address;
}

void rts(CPU *cpu, uint8_t *memory) {
    uint8_t low = read_memory(0x0100 + ++cpu->SP);  // Pop low byte from stack
    uint8_t high = read_memory(0x0100 + ++cpu->SP); // Pop high byte from stack
    cpu->PC = (high << 8) | low;
    cpu->PC++; // Increment PC after returning
}

void rti(CPU *cpu, uint8_t *memory) {
    cpu->status = read_memory(0x0100 + ++cpu->SP); // Pop status register
    uint8_t low = read_memory(0x0100 + ++cpu->SP);  // Pop low byte from stack
    uint8_t high = read_memory(0x0100 + ++cpu->SP); // Pop high byte from stack
    cpu->PC = (high << 8) | low;
}


// BMI - Branch if Minus (Negative flag set)
void bmi(CPU *cpu, uint8_t *memory) {
    int8_t offset = fetch(cpu, memory);
    if (CHECK_FLAG(cpu, FLAG_NEGATIVE)) {
        cpu->PC += offset;
    }
}

// BPL - Branch if Positive (Negative flag clear)
void bpl(CPU *cpu, uint8_t *memory) {
    int8_t offset = fetch(cpu, memory);
    if (!CHECK_FLAG(cpu, FLAG_NEGATIVE)) {
        cpu->PC += offset;
    }
}

// BVC - Branch if Overflow Clear
void bvc(CPU *cpu, uint8_t *memory) {
    int8_t offset = fetch(cpu, memory);
    if (!CHECK_FLAG(cpu, FLAG_OVERFLOW)) {
        cpu->PC += offset;
    }
}

// BVS - Branch if Overflow Set
void bvs(CPU *cpu, uint8_t *memory) {
    int8_t offset = fetch(cpu, memory);
    if (CHECK_FLAG(cpu, FLAG_OVERFLOW)) {
        cpu->PC += offset;
    }
}



void bit_zero_page(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory);
    uint8_t value = read_memory(address); // Use read_memory
    uint8_t result = cpu->A & value;

    if (result == 0) SET_FLAG(cpu, FLAG_ZERO);
    else CLEAR_FLAG(cpu, FLAG_ZERO);

    if (value & 0x80) SET_FLAG(cpu, FLAG_NEGATIVE);
    else CLEAR_FLAG(cpu, FLAG_NEGATIVE);

    if (value & 0x40) SET_FLAG(cpu, FLAG_OVERFLOW);
    else CLEAR_FLAG(cpu, FLAG_OVERFLOW);
}

void bit_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    uint8_t value = read_memory(address); // Use read_memory
    uint8_t result = cpu->A & value;

    if (result == 0) SET_FLAG(cpu, FLAG_ZERO);
    else CLEAR_FLAG(cpu, FLAG_ZERO);

    if (value & 0x80) SET_FLAG(cpu, FLAG_NEGATIVE);
    else CLEAR_FLAG(cpu, FLAG_NEGATIVE);

    if (value & 0x40) SET_FLAG(cpu, FLAG_OVERFLOW);
    else CLEAR_FLAG(cpu, FLAG_OVERFLOW);
}

void dec_zero_page(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory);
    uint8_t value = read_memory(address); // Use read_memory
    value--;
    write_memory(address, value); // Use write_memory
    update_zero_and_negative_flags(cpu, value);
}

void inc_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    uint8_t value = read_memory(address); // Use read_memory
    value++;
    write_memory(address, value); // Use write_memory
    update_zero_and_negative_flags(cpu, value);
}

void inc_zero_page(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory);
    uint8_t value = read_memory(address); 
    value++;
    write_memory(address, value); 
    update_zero_and_negative_flags(cpu, value);
}

void jmp_indirect(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);

    uint16_t indirect_address = read_memory(address);
    indirect_address |= (read_memory(address + 1) << 8);

    cpu->PC = indirect_address;
}

void asl_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    uint8_t value = read_memory(address); 

    if (value & 0x80) SET_FLAG(cpu, FLAG_CARRY);
    else CLEAR_FLAG(cpu, FLAG_CARRY);

    value <<= 1;
    write_memory(address, value); 
    update_zero_and_negative_flags(cpu, value);
}

void lsr_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    uint8_t value = read_memory(address); 
    if (value & 0x01) SET_FLAG(cpu, FLAG_CARRY);
    else CLEAR_FLAG(cpu, FLAG_CARRY);

    value >>= 1;
    write_memory(address, value);
    update_zero_and_negative_flags(cpu, value);
}

void rol_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    uint8_t value = read_memory(address);
    uint8_t carry_in = CHECK_FLAG(cpu, FLAG_CARRY);

    if (value & 0x80) SET_FLAG(cpu, FLAG_CARRY);
    else CLEAR_FLAG(cpu, FLAG_CARRY);

    value = (value << 1) | carry_in;
    write_memory(address, value);
    update_zero_and_negative_flags(cpu, value);
}

void ror_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    uint8_t value = read_memory(address); 
    uint8_t carry_in = CHECK_FLAG(cpu, FLAG_CARRY) ? 0x80 : 0x00;

    if (value & 0x01) SET_FLAG(cpu, FLAG_CARRY);
    else CLEAR_FLAG(cpu, FLAG_CARRY);

    value = (value >> 1) | carry_in;
    write_memory(address, value); 
    update_zero_and_negative_flags(cpu, value);
}

void eor_indexed_indirect(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory) + cpu->X;
    uint16_t effective_address = read_memory(address) | (read_memory(address + 1) << 8);
    cpu->A ^= read_memory(effective_address); 
    update_zero_and_negative_flags(cpu, cpu->A);
}

void eor_indirect_indexed(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory);
    uint16_t effective_address = read_memory(address) | (read_memory(address + 1) << 8);
    cpu->A ^= read_memory(effective_address + cpu->Y); 
    update_zero_and_negative_flags(cpu, cpu->A);
}

void ora_absolute_y(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory) | (fetch(cpu, memory) << 8);
    cpu->A |= read_memory(address + cpu->Y); 
    update_zero_and_negative_flags(cpu, cpu->A);
}

// ORA Zero Page
void ora_zero_page(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory);
    cpu->A |= read_memory(address); 
    update_zero_and_negative_flags(cpu, cpu->A);
}

// ASL Zero Page
void asl_zero_page(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory);
    uint8_t value = read_memory(address); 
    if (value & 0x80) SET_FLAG(cpu, FLAG_CARRY);
    else CLEAR_FLAG(cpu, FLAG_CARRY);

    value <<= 1;
    write_memory(address, value);
    update_zero_and_negative_flags(cpu, value);
}

// ALR (Unofficial)
void alr_immediate(CPU *cpu, uint8_t *memory) {
    cpu->A &= fetch(cpu, memory);
    cpu->A >>= 1;
    update_zero_and_negative_flags(cpu, cpu->A);
}

// ANC (Unofficial)
void anc_immediate(CPU *cpu, uint8_t *memory) {
    cpu->A &= fetch(cpu, memory);
    update_zero_and_negative_flags(cpu, cpu->A);
    if (cpu->A & 0x80) SET_FLAG(cpu, FLAG_CARRY);
    else CLEAR_FLAG(cpu, FLAG_CARRY);
}

void ora_absolute_x(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory) | (fetch(cpu, memory) << 8);
    cpu->A |= read_memory(address + cpu->X); 
    update_zero_and_negative_flags(cpu, cpu->A);
}

void nop_zero_page(CPU *cpu, uint8_t *memory) {
    fetch(cpu, memory); 
}

void eor_zero_page_x(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory) + cpu->X;
    cpu->A ^= read_memory(address);
    update_zero_and_negative_flags(cpu, cpu->A);
}

void eor_indirect(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory);
    uint16_t effective_address = read_memory(address) | (read_memory(address + 1) << 8);
    cpu->A ^= read_memory(effective_address); 
    update_zero_and_negative_flags(cpu, cpu->A);
}

void nop_zero_page_x(CPU *cpu, uint8_t *memory) {
    fetch(cpu, memory);
}


void brk(CPU *cpu, uint8_t *memory) {
    cpu->PC++; 
    SET_FLAG(cpu, FLAG_BREAK);

    uint16_t return_address = cpu->PC;
    write_memory(0x0100 + cpu->SP--, (return_address >> 8) & 0xFF); // Push high byte
    write_memory(0x0100 + cpu->SP--, return_address & 0xFF);        // Push low byte

    write_memory(0x0100 + cpu->SP--, cpu->status | FLAG_BREAK | FLAG_UNUSED);

    uint16_t irq_vector = read_memory(0xFFFE) | (read_memory(0xFFFF) << 8);

    if (irq_vector == 0x0000) {
        cpu->is_running = 0; 
    } else {
        cpu->PC = irq_vector;
    }

    CLEAR_FLAG(cpu, FLAG_BREAK);
}


void inc_absolute_x(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    
    address += cpu->X;
    
    uint8_t value = read_memory(address);
    
    value++;
    
    write_memory(address, value);
    
    update_zero_and_negative_flags(cpu, value);
}

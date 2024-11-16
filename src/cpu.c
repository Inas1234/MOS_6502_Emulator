#include "cpu.h"

void update_zero_and_negative_flags(CPU *cpu, uint8_t value) {
    if (value == 0)
        SET_FLAG(cpu, FLAG_ZERO);
    else
        CLEAR_FLAG(cpu, FLAG_ZERO);

    if (value & 0x80)
        SET_FLAG(cpu, FLAG_NEGATIVE);
    else
        CLEAR_FLAG(cpu, FLAG_NEGATIVE);
}


void reset_cpu(CPU * cpu) {
    cpu->A = 0;
    cpu->X = 0;
    cpu->Y = 0;
    cpu->SP = 0xFF;
    cpu->status = 0;
    cpu->PC = 0x600;
}

uint8_t fetch(CPU * cpu, uint8_t * memory) {
    return memory[cpu->PC++];
}

void execute(CPU *cpu, uint8_t *memory) {
    uint8_t opcode = fetch(cpu, memory);

    switch (opcode) {
        // Load/Store Instructions
        case 0xA9: lda_immediate(cpu, memory); break;
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
        case 0x00: brk(cpu); break;
        case 0xEA: nop(cpu); break;
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

        default:
            printf("Unknown opcode: 0x%02X\n", opcode);
            break;
    }
}


void lda_immediate(CPU *cpu, uint8_t *memory) {
    cpu->A = memory[cpu->PC++];
    if (cpu->A == 0) {
        SET_FLAG(cpu, FLAG_ZERO);
    }
    else {
        CLEAR_FLAG(cpu, FLAG_ZERO);
    }

    if (cpu->A & 0x80){
        SET_FLAG(cpu, FLAG_NEGATIVE);
    }
    else {
        CLEAR_FLAG(cpu, FLAG_NEGATIVE);
    }
}

void sta_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    memory[address] = cpu->A;
}

void adc_immediate(CPU *cpu, uint8_t *memory) {
    uint8_t value = fetch(cpu, memory);
    uint16_t result = cpu->A + value + (CHECK_FLAG(cpu, FLAG_CARRY) ? 1 : 0);

    if (result > 0xFF) {
        SET_FLAG(cpu, FLAG_CARRY);
    } else {
        CLEAR_FLAG(cpu, FLAG_CARRY);
    }

    if ((result & 0xFF) == 0) {
        SET_FLAG(cpu, FLAG_ZERO);
    } else {
        CLEAR_FLAG(cpu, FLAG_ZERO);
    }

    if (result & 0x80) {
        SET_FLAG(cpu, FLAG_NEGATIVE);
    } else {
        CLEAR_FLAG(cpu, FLAG_NEGATIVE);
    }

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

    if ((result & 0xFF) == 0) {
        SET_FLAG(cpu, FLAG_ZERO);
    } else {
        CLEAR_FLAG(cpu, FLAG_ZERO);
    }

    if (result & 0x80) {
        SET_FLAG(cpu, FLAG_NEGATIVE);
    } else {
        CLEAR_FLAG(cpu, FLAG_NEGATIVE);
    }

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
    memory[address] = cpu->X;
}

void sty_zero_page(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory);
    memory[address] = cpu->Y;
}

void cmp_immediate(CPU *cpu, uint8_t *memory) {
    uint8_t value = fetch(cpu, memory);
    uint16_t result = cpu->A - value;
    
    // Set flags
    if (cpu->A >= value)
        SET_FLAG(cpu, FLAG_CARRY);
    else
        CLEAR_FLAG(cpu, FLAG_CARRY);

    update_zero_and_negative_flags(cpu, (uint8_t)result);
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
    memory[0x0100 + cpu->SP--] = cpu->A;
}

void php(CPU *cpu, uint8_t *memory) {
    memory[0x0100 + cpu->SP--] = cpu->status | FLAG_BREAK | FLAG_UNUSED;
}

void pla(CPU *cpu, uint8_t *memory) {
    cpu->A = memory[0x0100 + ++cpu->SP];
    update_zero_and_negative_flags(cpu, cpu->A);
}

void plp(CPU *cpu, uint8_t *memory) {
    cpu->status = memory[0x0100 + ++cpu->SP] & ~FLAG_UNUSED;
}


void brk(CPU *cpu) {
    cpu->PC++;
    SET_FLAG(cpu, FLAG_BREAK);
    // Typically, BRK would push PC and status to the stack and jump to an interrupt vector.
}

void nop(CPU *cpu) {
    // Does nothing
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
    memory[address]--;
    update_zero_and_negative_flags(cpu, memory[address]);
}


void jmp_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    cpu->PC = address;
}

void jsr_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    cpu->SP -= 2;
    memory[0x0100 + cpu->SP + 1] = (cpu->PC >> 8) & 0xFF;
    memory[0x0100 + cpu->SP] = cpu->PC & 0xFF;
    cpu->PC = address;
}

void rts(CPU *cpu, uint8_t *memory) {
    uint8_t low = memory[0x0100 + ++cpu->SP];
    uint8_t high = memory[0x0100 + ++cpu->SP];
    cpu->PC = (high << 8) | low;
    cpu->PC++;
}

void rti(CPU *cpu, uint8_t *memory) {
    cpu->status = memory[0x0100 + ++cpu->SP];
    uint8_t low = memory[0x0100 + ++cpu->SP];
    uint8_t high = memory[0x0100 + ++cpu->SP];
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
    uint8_t value = memory[address];
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
    uint8_t value = memory[address];
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
    memory[address]--;
    update_zero_and_negative_flags(cpu, memory[address]);
}

void inc_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    memory[address]++;
    update_zero_and_negative_flags(cpu, memory[address]);
}

void inc_zero_page(CPU *cpu, uint8_t *memory) {
    uint8_t address = fetch(cpu, memory);
    memory[address]++;
    update_zero_and_negative_flags(cpu, memory[address]);
}

void jmp_indirect(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);

    uint16_t indirect_address = memory[address];
    indirect_address |= (memory[address + 1] << 8);

    cpu->PC = indirect_address;
}

void asl_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    uint8_t value = memory[address];

    if (value & 0x80) SET_FLAG(cpu, FLAG_CARRY);
    else CLEAR_FLAG(cpu, FLAG_CARRY);

    value <<= 1;
    memory[address] = value;
    update_zero_and_negative_flags(cpu, value);
}

void lsr_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    uint8_t value = memory[address];

    if (value & 0x01) SET_FLAG(cpu, FLAG_CARRY);
    else CLEAR_FLAG(cpu, FLAG_CARRY);

    value >>= 1;
    memory[address] = value;
    update_zero_and_negative_flags(cpu, value);
}

void rol_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    uint8_t value = memory[address];
    uint8_t carry_in = CHECK_FLAG(cpu, FLAG_CARRY);

    if (value & 0x80) SET_FLAG(cpu, FLAG_CARRY);
    else CLEAR_FLAG(cpu, FLAG_CARRY);

    value = (value << 1) | carry_in;
    memory[address] = value;
    update_zero_and_negative_flags(cpu, value);
}

void ror_absolute(CPU *cpu, uint8_t *memory) {
    uint16_t address = fetch(cpu, memory);
    address |= (fetch(cpu, memory) << 8);
    uint8_t value = memory[address];
    uint8_t carry_in = CHECK_FLAG(cpu, FLAG_CARRY) ? 0x80 : 0x00;

    if (value & 0x01) SET_FLAG(cpu, FLAG_CARRY);
    else CLEAR_FLAG(cpu, FLAG_CARRY);

    value = (value >> 1) | carry_in;
    memory[address] = value;
    update_zero_and_negative_flags(cpu, value);
}


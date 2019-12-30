#include "z80.h"
#include <stddef.h>
#include <stdio.h>


#define FLAG_C    0x01
#define FLAG_N    0x02
#define FLAG_P    0x04 // also it's V register
#define FLAG_3    0x08
#define FLAG_H    0x10
#define FLAG_5    0x20
#define FLAG_Z    0x40
#define FLAG_S    0x80

#define CB_SHIFT     0x01
#define ED_SHIFT     0x02
#define DD_SHIFT     0x03
#define FD_SHIFT     0x04
#define DISINT_SHIFT 0x08 // Disable interrupts for one (full) opcode after EI


uint8_t sz53_table[0x100]; // The S, Z, 5 and 3 bits of the index
uint8_t parity_table[0x100]; // The parity of the lookup value
uint8_t sz53p_table[0x100]; // OR the above two tables together
uint16_t daa_table[0x800]; // AF values for DAA operation

uint8_t halfcarry_add_table[8] = {0, FLAG_H, FLAG_H, FLAG_H,      0, 0,      0, FLAG_H};
uint8_t halfcarry_sub_table[8] = {0,      0, FLAG_H,      0, FLAG_H, 0, FLAG_H, FLAG_H};
uint8_t overflow_add_table[8] =  {0,      0,      0, FLAG_P, FLAG_P, 0,      0,      0};
uint8_t overflow_sub_table[8] =  {0, FLAG_P,      0,      0,      0, 0, FLAG_P,      0};


void z80_init(t_memory_reader mr, t_memory_writer mw, t_port_reader pr, t_port_writer pw)
{
    int i,j;
    uint8_t parity;

    for (i = 0; i < 0x100; i++) {
        sz53_table[i] = i & (FLAG_3 | FLAG_5 | FLAG_S);
        j = i;
        j ^= j >> 4;
        j ^= j >> 2;
        j ^= j >> 1;
        parity = j & 1;
        parity_table[i] = (parity ? 0 : FLAG_P);
        sz53p_table[i] = sz53_table[i] | parity_table[i];
    }
    sz53_table[0] |= FLAG_Z;
    sz53p_table[0] |= FLAG_Z;

    int af;
    for (af = 0; af < 0x800; af++) {
        uint8_t a = af & 0xff;
        uint8_t a_tmp = a;
        uint8_t f = ((af & 0x0400) ? FLAG_H : 0) | ((af & 0x0200) ? FLAG_N : 0) | ((af & 0x0100) ? FLAG_C : 0);
        uint8_t add = 0;
        uint8_t carry = (f & FLAG_C);

        if (((f & FLAG_H) != 0) || ((a & 0x0f) > 9)) {
            add = 6;
        }
        if ((carry != 0) || (a > 0x99)) {
            add |= 0x60;
        }
        if (a > 0x99) {
            carry = FLAG_C;
        }
        a_tmp += ((f & FLAG_N) ? -add : add);
        uint8_t tmp_f = (f & ~(FLAG_H | FLAG_C | FLAG_P)) | carry | parity_table[a_tmp] |
                        (a_tmp ? 0 : FLAG_Z) |
                        (a_tmp & (FLAG_3 | FLAG_5 | FLAG_S)) |
                        ((!(f & FLAG_N) && ((a & 0x0f) > 0x09)) ? FLAG_H : 0) |
                        (((f & FLAG_N) && (f & FLAG_H) && ((a & 0x0f) < 0x06)) ? FLAG_H : 0);
        daa_table[af] = (a_tmp << 8) | tmp_f;
    }

    z80.states = 0;
    z80.irq = false;

    z80.mr = mr;
    z80.mw = mw;
    z80.pr = pr;
    z80.pw = pw;
}


void z80_reset()
{
    z80.r16.af = z80.r16.af_ = 0xffff;
    z80.i = z80.r = 0;
    z80.pc = 0;
    z80.sp = 0xffff;
    z80.iff1 = z80.iff2 = 0;
    z80.im = IM0;
    z80.halted = false;
    z80.q = 0;

    z80.r16.bc = z80.r16.de = z80.r16.hl = 0;
    z80.r16.bc_ = z80.r16.de_= z80.r16.hl_ = 0;
    z80.r16.ix = z80.r16.iy = 0;
    z80.memptr = 0;

    z80.shifts = 0;
}


void z80_interrupt()
{


}

void load_byte(uint8_t *r8)
{
    *r8 = z80.mr(z80.pc++);
    z80.states += 4; // is it correct ?
}


void load_word(uint16_t *r16)
{
    *r16 = z80.mr(z80.pc) | (z80.mr(z80.pc + 1) << 8);
    z80.pc += 2;
    z80.states += 8; // is it correct ?
}


void push(uint16_t r16)
{
    z80.mw(z80.sp--, (r16 >> 8) & 0xff);
    z80.mw(z80.sp--, r16 & 0xff);
}


void pop(uint16_t *r16)
{
    *r16 = z80.mr(z80.sp + 1) | (z80.mr(z80.sp + 2) << 8);
    z80.sp += 2;
}


void add_r16_word(uint16_t *r16, uint16_t word)
{
    uint32_t tmp = *r16 + word;
    uint8_t lookup_byte = (uint8_t)(((*r16 & 0x0800) >> 11) | ((word & 0x0800) >> 10) | (tmp & 0x0800) >> 9);
    z80.memptr = *r16 + 1;
    *r16 = (uint16_t)(tmp);
    z80.r8.f = (z80.r8.f & (FLAG_P | FLAG_Z | FLAG_S)) | (((tmp & 0x10000) != 0) ? FLAG_C : 0) |
               (z80.r8.h & (FLAG_3 | FLAG_5)) | halfcarry_add_table[lookup_byte];
    z80.q = z80.r8.f;
}


void jr()
{
    int8_t jrtemp = z80.mr(z80.pc++);
    z80.states += 5;
    z80.pc += jrtemp;
    z80.memptr = z80.pc + 1;
}


 void jp(bool cond)
 {
    uint16_t tmp = z80.mr(z80.pc+1)  | (z80.mr(z80.pc+2) << 8);
    z80.pc += 2;
    if (cond) {
        z80.pc = tmp;
        // states ...
    }
    z80.memptr = tmp;
}


uint8_t *load_operand(uint8_t n, uint16_t *tmp_byte_address, uint8_t *tmp_byte)
{
    if (z80.shifts & DD_SHIFT) {
        switch (n) {
            case 0x00:
                return &z80.r8.b;
            case 0x01:
                return &z80.r8.c;
            case 0x02:
                return &z80.r8.d;
            case 0x03:
                return &z80.r8.e;
            case 0x04:
                return &z80.r8.ixh;
            case 0x05:
                return &z80.r8.ixl;
            case 0x06: {
                int8_t offset = z80.mr(z80.pc);
                z80.pc++;
                z80.states += 4; // is it correct ?
                *tmp_byte = z80.mr(*tmp_byte_address = z80.r16.ix + offset);
                return tmp_byte;
            }
            case 0x07:
                return &z80.r8.a;
        }
    }
    if (z80.shifts & FD_SHIFT) {
        switch (n) {
            case 0x00:
                return &z80.r8.b;
            case 0x01:
                return &z80.r8.c;
            case 0x02:
                return &z80.r8.d;
            case 0x03:
                return &z80.r8.e;
            case 0x04:
                return &z80.r8.iyh;
            case 0x05:
                return &z80.r8.iyl;
            case 0x06: {
                int8_t offset = z80.mr(z80.pc);
                z80.pc++;
                z80.states += 4; // is it correct ?
                *tmp_byte = z80.mr(*tmp_byte_address = z80.r16.iy + offset);
                return tmp_byte;
            }
            case 0x07:
                return &z80.r8.a;
        }
    }
    switch (n) {
        case 0x00:
            return &z80.r8.b;
        case 0x01:
            return &z80.r8.c;
        case 0x02:
            return &z80.r8.d;
        case 0x03:
            return &z80.r8.e;
        case 0x04:
            return &z80.r8.h;
        case 0x05:
            return &z80.r8.l;
        case 0x06:
            *tmp_byte = z80.mr(*tmp_byte_address = z80.r16.hl);
            return tmp_byte;
        case 0x07:
            return &z80.r8.a;
    }
    return NULL;
}


void store_operand(uint8_t n, uint8_t tmp_byte)
{
   if (z80.shifts & DD_SHIFT) {
        switch (n) {
            case 0x00:
                z80.r8.b = tmp_byte;
                return;
            case 0x01:
                z80.r8.c = tmp_byte;
                return;
            case 0x02:
                z80.r8.d = tmp_byte;
                return;
            case 0x03:
                z80.r8.e = tmp_byte;
                return;
            case 0x04:
                z80.r8.ixh = tmp_byte;
                return;
            case 0x05:
                z80.r8.ixl = tmp_byte;
                return;
            case 0x06: {
                int8_t offset = z80.mr(z80.pc);
                z80.pc++;
                z80.states += 4; // is it correct ?
                z80.mw(z80.r16.ix + offset, tmp_byte);
                return;
            }
            case 0x07:
                z80.r8.a = tmp_byte;
                return;
        }
    }
    if (z80.shifts & FD_SHIFT) {
        switch (n) {
            case 0x00:
                z80.r8.b = tmp_byte;
                return;
            case 0x01:
                z80.r8.c = tmp_byte;
                return;
            case 0x02:
                z80.r8.d = tmp_byte;
                return;
            case 0x03:
                z80.r8.e = tmp_byte;
                return;
            case 0x04:
                z80.r8.iyh = tmp_byte;
                return;
            case 0x05:
                z80.r8.iyl = tmp_byte;
                return;
            case 0x06: {
                int8_t offset = z80.mr(z80.pc);
                z80.pc++;
                z80.states += 4; // is it correct ?
                z80.mw(z80.r16.iy + offset, tmp_byte);
                return;
            }
            case 0x07:
                z80.r8.a = tmp_byte;
                return;
        }
    }
    switch (n) {
        case 0x00:
            z80.r8.b = tmp_byte;
            return;
        case 0x01:
            z80.r8.c = tmp_byte;
            return;
        case 0x02:
            z80.r8.d = tmp_byte;
            return;
        case 0x03:
            z80.r8.e = tmp_byte;
            return;
        case 0x04:
            z80.r8.h = tmp_byte;
            return;
        case 0x05:
            z80.r8.l = tmp_byte;
            return;
        case 0x06:
            z80.mw(z80.r16.hl, tmp_byte);
            return;
        case 0x07:
            z80.r8.a = tmp_byte;
            return;
    }
}


uint8_t *cb_load_operand(uint8_t n, uint16_t *tmp_byte_address, uint8_t *tmp_byte)
{
    if (z80.shifts & DD_SHIFT) {
        int8_t offset = z80.mr(z80.pc);
        z80.pc++;
        z80.states += 4; // is it correct ?
        *tmp_byte = z80.mr(*tmp_byte_address = z80.r16.ix + offset);
        return tmp_byte;
    }
    if (z80.shifts & FD_SHIFT) {
        int8_t offset = z80.mr(z80.pc);
        z80.pc++;
        z80.states += 4; // is it correct ?
        *tmp_byte = z80.mr(*tmp_byte_address = z80.r16.iy + offset);
        return tmp_byte;
    }
    switch (n) {
        case 0x00:
            return &z80.r8.b;
        case 0x01:
            return &z80.r8.c;
        case 0x02:
            return &z80.r8.d;
        case 0x03:
            return &z80.r8.e;
        case 0x04:
            return &z80.r8.h;
        case 0x05:
            return &z80.r8.l;
        case 0x06:
            *tmp_byte = z80.mr(*tmp_byte_address = z80.r16.hl);
            return tmp_byte;
        case 0x07:
            return &z80.r8.a;
    }
    return NULL;
}


void cb_store_operand(uint8_t n, uint16_t tmp_byte_address, uint8_t tmp_byte)
{
    if (n == 0x06) {
        z80.mw(tmp_byte_address, tmp_byte);
    }
}


void z80_opcocde()
{
    uint8_t command;
    uint16_t tmp_byte_address;
    uint8_t tmp_byte;

    command = z80.mr(z80.pc);

    if (!z80.shifts && z80.irq /* && interrupts_enabled */) {

        z80_interrupt();
    }

    if (z80.shifts & CB_SHIFT) {
        uint8_t *r;
        uint8_t bit;

        r = cb_load_operand(command & 0x07, &tmp_byte_address, &tmp_byte);
        bit = ((command & 0x38) >> 3);

        switch (command & 0xf8) {
            case 0x00:
                // RLC (r)
                *r = (*r << 1) | (*r >> 7);
                z80.r8.f = (*r & FLAG_C) | sz53p_table[*r];
                z80.q = z80.r8.f;
                break;
            case 0x08:
                // RRC (r)
                z80.r8.f = *r & FLAG_C;
                *r = (*r >> 1) | (*r << 7);
                z80.r8.f |= sz53p_table[*r];
                z80.q = z80.r8.f;
                break;
            case 0x10: {
                // RL (r)
                uint8_t tmp = *r;
                *r = (*r << 1) | (z80.r8.f & FLAG_C);
                z80.r8.f = (tmp >> 7) | sz53p_table[*r];
                z80.q = z80.r8.f;
                break;
            }
            case 0x18: {
                // RR (r)
                uint8_t tmp = *r;
                *r = (*r >> 1) | (z80.r8.f << 7);
                z80.r8.f = (tmp & FLAG_C) | sz53p_table[*r];
                z80.q = z80.r8.f;
                break;
            }
            case 0x20:
                // SLA (r)
                z80.r8.f = *r >> 7;
                *r <<= 1;
                z80.r8.f |= sz53p_table[*r];
                z80.q = z80.r8.f;
                break;
            case 0x28:
                // SRA (r)
                z80.r8.f = *r & FLAG_C;
                *r = (*r & 0x80) | (*r >> 1);
                z80.r8.f |= sz53p_table[*r];
                z80.q = z80.r8.f;
                break;
            case 0x30:
                // SLL (r)
                z80.r8.f = *r >> 7;
                *r = (*r << 1) | 0x01;
                z80.r8.f |= sz53p_table[*r];
                z80.q = z80.r8.f;
                break;
            case 0x38:
                // SRL (r)
                z80.r8.f = *r & FLAG_C;
                *r >>= 1;
                z80.r8.f |= sz53p_table[*r];
                z80.q = z80.r8.f;
                break;
        }

        switch (command & 0xc0) {
            case 0x40:
                // BIT (bit), (r)
                z80.r8.f = (z80.r8.f & FLAG_C) | FLAG_H | (*r & (FLAG_3 | FLAG_5));
                if ((*r & (0x01 << bit)) == 0) {
                    z80.r8.f |= FLAG_P | FLAG_Z;
                }
                if ((bit == 7) && ((*r & 0x80) != 0)) {
                    z80.r8.f |= FLAG_S;
                }
                z80.q = z80.r8.f;
                break;
            case 0x80:
                // RES (bit), (r)
                *r &= ~(0x01 << bit);
                z80.q = 0;
                break;
            case 0xc0:
                // SET (bit), (r)
                *r |= (0x01 << bit);
                z80.q = 0;
                break;
        }

        cb_store_operand(command & 0x07, tmp_byte_address, tmp_byte);

        z80.shifts = 0;
        return;

    } else if (z80.shifts & ED_SHIFT) {

        switch (command) {
            case 0x40:
                // IN B,(C)
                break;
            case 0x41:
                // OUT (C),B
                break;
            case 0x42:
                // SBC HL,BC
                break;
            case 0x43:
                // LD (nnnn),BC
                break;
            case 0x7c:
                // NEG
                break;
            case 0x7d:
                // RETN
                break;
            case 0x6e:
                // IM 0
                break;
            case 0x47:
                // LD I,A
                break;
            case 0x48:
                // IN C,(C)
                break;
            case 0x49:
                // OUT (C),C
                break;
            case 0x4a:
                // ADC HL,BC
                break;
            case 0x4b:
                // LD BC,(nnnn)
                break;
            case 0x4f:
                // LD R,A
                break;
            case 0x50:
                // IN D,(C)
                break;
            case 0x51:
                // OUT (C),D
                break;
            case 0x52:
                // SBC HL,DE
                break;
            case 0x53:
                // LD (nnnn),DE
                break;
            case 0x76:
                // IM 1
                break;
            case 0x57:
                // LD A,I
                break;
            case 0x58:
                // IN E,(C)
                break;
            case 0x59:
                // OUT (C),E
                break;
            case 0x5a:
                // ADC HL,DE
                break;
            case 0x5b:
                // LD DE,(nnnn)
                break;
            case 0x7e:
                // IM 2
                break;
            case 0x5f:
                // LD A,R
                break;
            case 0x60:
                // IN H,(C)
                break;
            case 0x61:
                // OUT (C),H
                break;
            case 0x62:
                // SBC HL,HL
                break;
            case 0x63:
                // LD (nnnn),HL
                break;
            case 0x67:
                // RRD
                break;
            case 0x68:
                // IN L,(C)
                break;
            case 0x69:
                // OUT (C),L
                break;
            case 0x6a:
                // ADC HL,HL
                break;
            case 0x6b:
                // LD HL,(nnnn)
                break;
            case 0x6f:
                // RLD
                break;
            case 0x70:
                // IN F,(C)
                break;
            case 0x71:
                // OUT (C),0
                break;
            case 0x72:
                // SBC HL,SP
                break;
            case 0x73:
                // LD (nnnn),SP
                break;
            case 0x78:
                // IN A,(C)
                break;
            case 0x79:
                // OUT (C),A
                break;
            case 0x7a:
                // ADC HL,SP
                break;
            case 0x7b:
                // LD SP,(nnnn)
                break;
            case 0xa0:
                // LDI
                break;
            case 0xa1:
                // CPI
                break;
            case 0xa2:
                // INI
                break;
            case 0xa3:
                // OUTI
                break;
            case 0xa8:
                // LDD
                break;
            case 0xa9:
                // CPD
                break;
            case 0xaa:
                // IND
                break;
            case 0xab:
                // OUTD
                break;
            case 0xb0:
                // LDIR
                break;
            case 0xb1:
                // CPIR
                break;
            case 0xb2:
                // INIR
                break;
            case 0xb3:
                // OTIR
                break;
            case 0xb8:
                // LDDR
                break;
            case 0xb9:
                // CPDR
                break;
            case 0xba:
                // INDR
                break;
            case 0xbb:
                // OTDR
                break;
        }
        z80.shifts = 0;
        return;
    } else { // no cb or ed shift

        switch (command & 0xc0) {
            case 0x00:

                if (((command & 0x07) >= 0x04) && ((command & 0x07) <= 0x06)) {
                    uint8_t *r;

                    switch (command & 0x07) {
                        case 0x04:
                            // INC (r)
                            r = load_operand((command >> 3) & 0x07, &tmp_byte_address, &tmp_byte);
                            (*r)++;
                            z80.r8.f = (z80.r8.f & FLAG_C) | ((*r == 0x80) ? FLAG_P : 0) | (((*r & 0x0f) != 0) ? 0 : FLAG_H) | sz53_table[*r];
                            z80.q = z80.r8.f;
                            store_operand((command >> 3) & 0x07, *r);
                            break;
                        case 0x05:
                            // DEC (r)
                            r = load_operand((command >> 3) & 0x07, &tmp_byte_address, &tmp_byte);
                            z80.r8.f = (z80.r8.f & FLAG_C) | (((*r & 0x0f) != 0) ? 0 : FLAG_H) | FLAG_N;
                            (*r)--;
                            z80.r8.f |= ((*r == 0x7f) ? FLAG_P : 0) | sz53_table[*r];
                            z80.q = z80.r8.f;
                            store_operand((command >> 3) & 0x07, *r);
                            break;
                        case 0x06: {
                            // LD (r),nn
                            uint8_t tmp;
                            load_byte(&tmp);
                            z80.q = 0;
                            store_operand((command >> 3) & 0x07, tmp);
                            break;
                        }
                    }
                } else {
                    switch (command) {
                        case 0x00:
                            // 0x00 NOP
                            z80.q = 0;
                            break;
                        case 0x01:
                            // LD BC,nnnn
                            load_word(&z80.r16.bc);
                            z80.q = 0;
                            break;
                        case 0x02:
                            // LD (BC),A
                            z80.mw(z80.r16.bc, z80.r8.a);
                            z80.memptr = (z80.r16.af & 0xff00) | ((z80.r16.bc + 1) & 0x00ff);
                            z80.q = 0;
                            break;
                        case 0x03:
                            // INC BC
                            z80.states += 2;
                            z80.r16.bc++;
                            z80.q = 0;
                            break;
                        case 0x07:
                            // RLCA
                            z80.r8.a = (z80.r8.a << 1) | (z80.r8.a >> 7);
                            z80.r8.f = (z80.r8.f & (FLAG_P | FLAG_Z | FLAG_S)) | (z80.r8.a & (FLAG_C | FLAG_3 | FLAG_5));
                            z80.q = z80.r8.f;
                            break;
                        case 0x08: {
                            // EX AF,AF'
                            uint16_t old_af = z80.r16.af;
                            z80.r16.af = z80.r16.af_;
                            z80.r16.af_ = old_af;
                            z80.q = 0;
                            break;
                        }
                        case 0x09:
                            // ADD HL,BC // if shifted then IX / IY
                            add_r16_word((z80.shifts & DD_SHIFT) ? &z80.r16.ix : ((z80.shifts & FD_SHIFT) ? &z80.r16.iy : &z80.r16.hl), z80.r16.bc);
                            z80.states += 7;
                            break;
                        case 0x0a:
                            // LD A,(BC)
                            z80.r8.a = z80.mr(z80.r16.bc);
                            z80.memptr = z80.r16.bc + 1;
                            z80.q = 0;
                            break;
                        case 0x0b:
                            // DEC BC
                            z80.states += 2;
                            z80.r16.bc--;
                            z80.q = 0;
                            break;
                        case 0x0f:
                            // RRCA
                            z80.r8.f = (z80.r8.f & (FLAG_P | FLAG_Z | FLAG_S)) | (z80.r8.a & FLAG_C);
                            z80.r8.a = (z80.r8.a >> 1) | (z80.r8.a << 7);
                            z80.r8.f |= (z80.r8.a & (FLAG_3 | FLAG_5));
                            z80.q = z80.r8.f;
                            break;
                        case 0x10:
                            // DJNZ offset
                            z80.r8.b--;
                            if (z80.r8.b != 0) {
                                jr();
                            } else {
                                z80.pc++;
                                z80.q = 0;
                            }
                            break;
                        case 0x11:
                            // LD DE,nnnn
                            load_word(&z80.r16.de);
                            z80.q = 0;
                            break;
                        case 0x12:
                            // LD (DE),A
                            z80.mw(z80.r16.de, z80.r8.a);
                            z80.memptr = (z80.r16.af & 0xff00) | ((z80.r16.de + 1) & 0x00ff);
                            z80.q = 0;
                            break;
                        case 0x13:
                            // INC DE
                            z80.states += 2;
                            z80.r16.de++;
                            z80.q = 0;
                            break;
                        case 0x17: {
                            // RLA
                            uint8_t tmp = z80.r8.a;
                            z80.r8.a = (z80.r8.a << 1) | (z80.r8.f & FLAG_C);
                            z80.r8.f = (z80.r8.f & (FLAG_P | FLAG_Z | FLAG_S)) | (z80.r8.a & (FLAG_3 | FLAG_5)) | (tmp >> 7);
                            z80.q = z80.r8.f;
                            break;
                        }
                        case 0x18:
                            // JR offset
                            jr();
                            z80.q = 0;
                            break;
                        case 0x19:
                            // ADD HL,DE  // if shifted then IX / IY
                            add_r16_word((z80.shifts & DD_SHIFT) ? &z80.r16.ix : ((z80.shifts & FD_SHIFT) ? &z80.r16.iy : &z80.r16.hl), z80.r16.de);
                            z80.states += 7;
                            break;
                        case 0x1a:
                            // LD A,(DE)
                            z80.r8.a = z80.mr(z80.r16.de);
                            z80.memptr = z80.r16.de + 1;
                            z80.q = 0;
                            break;
                        case 0x1b:
                            // DEC DE
                            z80.states += 2;
                            z80.r16.de--;
                            z80.q = 0;
                            break;
                        case 0x1f: {
                            // RRA
                            uint8_t tmp = z80.r8.a;
                            z80.r8.a = (z80.r8.a >> 1) | (z80.r8.f << 7);
                            z80.r8.f = (z80.r8.f & (FLAG_P | FLAG_Z | FLAG_S)) | (z80.r8.a & (FLAG_3 | FLAG_5)) | (tmp & FLAG_C);
                            z80.q = z80.r8.f;
                            break;
                        }
                        case 0x20:
                            // JR NZ,offset
                            if (!(z80.r8.f & FLAG_Z)) {
                                jr();
                            } else {
                                z80.states += 3;
                                z80.pc++;
                            }
                            z80.q = 0;
                            break;
                        case 0x21:
                            // LD HL,nnnn // if shifted then IX / IY
                            load_word((z80.shifts & DD_SHIFT) ? &z80.r16.ix : ((z80.shifts & FD_SHIFT) ? &z80.r16.iy : &z80.r16.hl));
                            z80.q = 0;
                            break;
                        case 0x22: {
                            // LD (nnnn),HL // if shifted then IX / IY
                            uint16_t tmp;
                            load_word(&tmp);
                            z80.mw(tmp, (z80.shifts & DD_SHIFT) ? z80.r8.ixl : ((z80.shifts & FD_SHIFT) ? z80.r8.iyl : z80.r8.l));
                            tmp++;
                            z80.mw(tmp, (z80.shifts & DD_SHIFT) ? z80.r8.ixh : ((z80.shifts & FD_SHIFT) ? z80.r8.iyh : z80.r8.h));
                            z80.memptr = tmp; // ++ ??????????????????????????
                            z80.q = 0;
                            break;
                        }
                        case 0x23:
                            // INC HL // if shifted then IX / IY
                            (z80.shifts & DD_SHIFT) ? z80.r16.ix++ : ((z80.shifts & FD_SHIFT) ? z80.r16.iy++ : z80.r16.hl++);
                            z80.states += 2;
                            z80.q = 0;
                            break;
                        case 0x27: {
                            // DAA
                            z80.r16.af = daa_table[z80.r8.a + 0x100 * ((z80.r8.f & 3) + ((z80.r8.f >> 2) & 4))];
                            z80.q = z80.r8.f;
                            break;
                        }
                        case 0x28:
                            // JR Z,offset
                            if (z80.r8.f & FLAG_Z) {
                                jr();
                            } else {
                                z80.states += 3;
                                z80.pc++;
                            }
                            z80.q = 0;
                            break;
                        case 0x29:
                            // ADD HL,HL // if shifted then IX / IY (both)
                            add_r16_word((z80.shifts & DD_SHIFT) ? &z80.r16.ix : ((z80.shifts & FD_SHIFT) ? &z80.r16.iy : &z80.r16.hl),
                                         (z80.shifts & DD_SHIFT) ? z80.r16.ix : ((z80.shifts & FD_SHIFT) ? z80.r16.iy : z80.r16.hl));
                            z80.states += 7;
                            break;
                        case 0x2a: {
                            // LD HL,(nnnn) // if shifted then IX / IY
                            uint16_t tmp;
                            uint8_t *regl = (z80.shifts & DD_SHIFT) ? &z80.r8.ixl : ((z80.shifts & FD_SHIFT) ? &z80.r8.iyl : &z80.r8.l);
                            uint8_t *regh = (z80.shifts & DD_SHIFT) ? &z80.r8.ixh : ((z80.shifts & FD_SHIFT) ? &z80.r8.iyh : &z80.r8.h);
                            load_word(&tmp);
                            *regl = z80.mr(tmp);
                            tmp++;
                            *regh = z80.mr(tmp);
                            z80.memptr = tmp;
                            z80.q = 0;
                            break;
                        }
                        case 0x2b:
                            // DEC HL // if shifted then IX / IY
                            (z80.shifts & DD_SHIFT) ? z80.r16.ix-- : ((z80.shifts & FD_SHIFT) ? z80.r16.iy-- : z80.r16.hl--);
                            z80.states += 2;
                            z80.q = 0;
                            break;
                        case 0x2f:
                            // CPL
                            z80.r8.a ^= 0xff;
                            z80.r8.f = (z80.r8.f & (FLAG_C | FLAG_P | FLAG_Z | FLAG_S)) |
                                       (z80.r8.a & (FLAG_3 | FLAG_5)) |
                                       (FLAG_N | FLAG_H);
                            z80.q = z80.r8.f;
                            break;
                        case 0x30:
                            // JR NC,offset
                            if (!(z80.r8.f & FLAG_C)) {
                                jr();
                            } else {
                                z80.states += 3;
                                z80.pc++;
                            }
                            z80.q = 0;
                            break;
                        case 0x31:
                            // LD SP,nnnn
                            load_word(&z80.sp);
                            z80.q = 0;
                            break;
                        case 0x32: {
                            // LD (nnnn),A
                            uint16_t tmp;
                            load_word(&tmp);
                            z80.mw(tmp, z80.r8.a);
                            z80.memptr = (z80.r16.af & 0xff00) | ((tmp + 1) & 0x00ff);
                            z80.q = 0;
                            break;
                        }
                        case 0x33:
                            // INC SP
                            z80.sp++;
                            z80.states += 2;
                            z80.q = 0;
                            break;
                        case 0x37:
                            // SCF
                            z80.r8.f = (z80.r8.f & (FLAG_P | FLAG_Z | FLAG_S)) |
                                       (((z80.q ^ z80.r8.f) | z80.r8.a) & (FLAG_3 | FLAG_5)) | FLAG_C;
                            z80.q = z80.r8.f;
                            break;
                        case 0x38:
                            // JR C,offset
                            if (z80.r8.f & FLAG_C) {
                                jr();
                            } else {
                                z80.states +=3;
                                z80.pc++;
                            }
                            z80.q = 0;
                            break;
                        case 0x39:
                            // ADD HL,SP // if shifted then IX / IY
                            add_r16_word((z80.shifts & DD_SHIFT) ? &z80.r16.ix : ((z80.shifts & FD_SHIFT) ? &z80.r16.iy : &z80.r16.hl), z80.sp);
                            z80.states += 7;
                            break;
                        case 0x3a: {
                            // LD A,(nnnn)
                            uint16_t tmp;
                            load_word(&tmp);
                            z80.r8.a = z80.mr(tmp);
                            z80.memptr = tmp + 1;
                            z80.q = 0;
                            break;
                        }
                        case 0x3b:
                            // DEC SP
                            z80.sp--;
                            z80.states += 2;
                            z80.q = 0;
                            break;
                        case 0x3f:
                            // CCF
                            z80.r8.f = (z80.r8.f & (FLAG_P | FLAG_Z | FLAG_S)) |
                                       (((z80.r8.f & FLAG_C) != 0) ? FLAG_H : FLAG_C) |
                                       (((z80.q ^ z80.r8.f) | z80.r8.a) & (FLAG_3 | FLAG_5));
                            z80.q = z80.r8.f;
                            break;
                    }
                }
            case 0x40:

                if (command == 0x76) {
                    // HALT
                    //.......................................................................!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                } else {
                    // LD (r),(r)
                    uint8_t *r = load_operand(command & 0x07, &tmp_byte_address, &tmp_byte);
                    store_operand((command >> 3) & 0x07, *r);
                    z80.q = 0;
                }
                break;

            case 0x80: {
                uint8_t *r = load_operand(command & 0x07, &tmp_byte_address, &tmp_byte);

                switch (command & 0xf8) {
                    case 0x80: {
                        // ADD A,(r)
                        uint16_t tmp = (uint16_t)z80.r8.a + *r;
                        uint8_t lookup_byte = ((z80.r8.a & 0x88) >> 3) | ((*r & 0x88) >> 2) | (uint8_t)((tmp & 0x88) >> 1);
                        z80.r8.a = tmp;
                        z80.r8.f = (((tmp & 0x100) != 0) ? FLAG_C : 0) | halfcarry_add_table[lookup_byte & 0x07] |
                                   overflow_add_table[lookup_byte >> 4] | sz53_table[z80.r8.a];
                        z80.q = z80.r8.f;
                        break;
                    }
                    case 0x88: {
                        // ADC A,(r)
                        uint16_t tmp = (uint16_t)z80.r8.a + *r + ((uint16_t)z80.r8.f & FLAG_C);
                        uint8_t lookup_byte = (((uint16_t)z80.r8.a & 0x88) >> 3) | ((*r & 0x88) >> 2) | (((uint16_t)tmp & 0x88) >> 1);
                        z80.r8.a = tmp;
                        z80.r8.f = (((tmp & 0x100) != 0) ? FLAG_C : 0) | halfcarry_add_table[lookup_byte & 0x07] |
                                   overflow_add_table[lookup_byte >> 4] | sz53_table[z80.r8.a];
                        z80.q = z80.r8.f;
                        break;
                    }
                    case 0x90: {
                        // SUB A,(r)
                        uint16_t tmp = (uint16_t)z80.r8.a - *r;
                        uint8_t lookup_byte = ((z80.r8.a & 0x88) >> 3) | ((*r & 0x88) >> 2) | (uint8_t)((tmp & 0x88) >> 1);
                        z80.r8.a = tmp;
                        z80.r8.f = (((tmp & 0x100) != 0) ? FLAG_C : 0) | FLAG_N | halfcarry_sub_table[lookup_byte & 0x07] |
                                   overflow_sub_table[lookup_byte >> 4] | sz53_table[z80.r8.a];
                        z80.q = z80.r8.f;
                        break;
                    }
                    case 0x98: {
                        // SBC A,(r)
                        uint16_t tmp  = (uint16_t)z80.r8.a - *r - ((uint16_t)z80.r8.f & FLAG_C);
                        uint8_t lookup_byte = ((z80.r8.a & 0x88) >> 3) | ((*r & 0x88) >> 2) | (uint8_t)((tmp & 0x88) >> 1);
                        z80.r8.a = tmp;
                        z80.r8.f = (((tmp & 0x100) != 0) ? FLAG_C : 0) | FLAG_N | halfcarry_sub_table[lookup_byte & 0x07] |
                                   overflow_sub_table[lookup_byte >> 4] | sz53_table[z80.r8.a];
                        z80.q = z80.r8.f;
                        break;
                    }
                    case 0xa0:
                        // AND A,(r)
                        z80.r8.a &= *r;
                        z80.r8.f = FLAG_H | sz53p_table[z80.r8.a];
                        z80.q = z80.r8.f;
                        break;
                    case 0xa8:
                        // XOR A,(r)
                        z80.r8.a ^= *r;
                        z80.r8.f = sz53p_table[z80.r8.a];
                        z80.q = z80.r8.f;
                        break;
                    case 0xb0:
                        // OR A,(r)
                        z80.r8.a |= *r;
                        z80.r8.f = sz53p_table[z80.r8.a];
                        z80.q = z80.r8.f;
                        break;
                    case 0xb8: {
                        // CP (r)
                        uint16_t tmp = (uint16_t)z80.r8.a - *r;
                        uint8_t lookup_byte = ((z80.r8.a & 0x88) >> 3) | ((*r & 0x88) >> 2) | (uint8_t)((tmp & 0x88) >> 1);
                        z80.r8.f = (((tmp & 0x100) != 0) ? FLAG_C : ((tmp != 0) ? 0 : FLAG_Z)) | FLAG_N | halfcarry_sub_table[lookup_byte & 0x07] |
                                   overflow_sub_table[lookup_byte >> 4] | (*r & (FLAG_3 | FLAG_5)) | (uint8_t)(tmp & FLAG_S);
                        z80.q = z80.r8.f;
                        break;
                    }
                }
            }

            case 0xc0: {
                switch (command & 0x07) {
                    // FLAG = (NZ Z NC C PO PE P M)
                    case 0x00:
                        // RET (FLAG)
                        break;
                    case 0x02:
                        // JP (FLAG),nnnn
                        break;
                    case 0x04:
                        // CALL (FLAG),nnnn
                        break;
                    case 0x07: {
                        // RST (address)
                        uint16_t tmp = (command & 0x38);
                        push(z80.pc);
                        z80.pc = tmp;
                        z80.memptr = z80.pc;
                        z80.q = 0;
                        break;
                    }
                }
                switch (command) {
                    case 0xc1:
                        // POP BC
                        pop(&z80.r16.bc);
                        z80.q = 0;
                        break;
                    case 0xc3:
                        // JP nnnn
                        jp(true);
                        z80.q = 0;
                        break;
                    case 0xc5:
                        // PUSH BC
                        push(z80.r16.bc);
                        z80.q = 0;
                        break;
                    case 0xc6:
                        // ADD A,nn
                        break;
                    case 0xc9:
                        // RET
                        break;
                    case 0xcb:
                        // shift CB
                        z80.shifts |= CB_SHIFT;
                        return; // !!!
                    case 0xcd:
                        // CALL nnnn
                        break;
                    case 0xce:
                        // ADC A,nn
                        break;
                    case 0xd1:
                        // POP DE
                        pop(&z80.r16.de);
                        z80.q = 0;
                        break;
                    case 0xd3:
                        // OUT (nn),A
                        break;
                    case 0xd5:
                        // PUSH DE
                        push(z80.r16.de);
                        z80.q = 0;
                        break;
                    case 0xd6:
                        // SUB nn
                        break;
                    case 0xd9:
                        // EXX
                        break;
                    case 0xdb:
                        // IN A,(nn)
                        break;
                    case 0xdd:
                        // shift DD
                        z80.shifts |= DD_SHIFT;
                        return; // !!!
                    case 0xde:
                        // SBC A,nn
                        break;
                    case 0xe1:
                        // POP HL // if shifted then IX / IY
                        pop((z80.shifts & DD_SHIFT) ? &z80.r16.ix : ((z80.shifts & FD_SHIFT) ? &z80.r16.iy : &z80.r16.hl));
                        z80.q = 0;
                        break;
                    case 0xe3:
                        // EX (SP),HL // if shifted then IX / IY
                        break;
                    case 0xe5:
                        // PUSH HL // if shifted then IX / IY
                        push((z80.shifts & DD_SHIFT) ? z80.r16.ix : ((z80.shifts & FD_SHIFT) ? z80.r16.iy : z80.r16.hl));
                        z80.q = 0;
                        break;
                    case 0xe6:
                        // AND nn
                        break;
                    case 0xe9:
                        // JP HL // if shifted then IX / IY
                        break;
                    case 0xeb:
                        // EX DE,HL
                        break;
                    case 0xed:
                        // shift ED
                        z80.shifts |= ED_SHIFT;
                        return; // !!!
                    case 0xee:
                        // XOR A,nn
                        break;
                    case 0xf1:
                        // POP AF
                        pop(&z80.r16.af);
                        z80.q = 0;
                        break;
                    case 0xf3:
                        // DI
                        break;
                    case 0xf5:
                        // PUSH AF
                        push(z80.r16.af);
                        z80.q = 0;
                        break;
                    case 0xf6:
                        // OR nn
                        break;
                    case 0xf9:
                        // LD SP,HL // if shifted then IX / IY
                        break;
                    case 0xfb:
                        // EI
                        z80.shifts |= DISINT_SHIFT;
                        return; // !!!
                    case 0xfd:
                        z80.shifts |= FD_SHIFT;
                        return; // !!!
                    case 0xfe:
                        // CP nn
                        break;
                }
            }
        }
    }
}

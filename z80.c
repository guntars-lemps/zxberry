

typedef struct t_r16
{
    uint16 af, bc, de, hl, ix, iy;
};

# if defined __BIG_ENDIAN__

typedef struct t_r8
{
    uint8 a, f, b, c, d, e, h, l, ixh, x, iyh, y;
};

#else

typedef struct t_r8
{
    uint8 f, a, c, b, e, d, l, h, x, iyh, y, iyh;
};

#endif

struct z80
{
    uint16 pc, sp, memptr;
    uint8 i, r;
    uint8 iff1, iff2;
    union
    {
        t_r16 r16;
        t_r8 r8;
    }
}

void z80_init()
{
    int i,j,k;
    uint8 parity;

    for (i = 0; i < 0x100; i++) {
        sz53_table[i] = i & (FLAG_3 | FLAG_5 | FLAG_S);
        j = i;
        parity = 0;
        for (k = 0; k < 8; k++) {
            parity ^= j & 1;
            j >>= 1;
        }
        parity_table[i] = (parity ? 0 : FLAG_P);
        sz53p_table[i] = sz53_table[i] | parity_table[i];
    }
    sz53_table[0] |= FLAG_Z;
    sz53p_table[0] |= FLAG_Z;
}


void z80_reset()
{
    z80.r16.af = z80.r16.af_ = 0xffff;
    z80.i = z80.r = z80.r7 = 0;
    z80.pc = 0;
    z80.sp = 0xffff;
    z80.iff1 = z80.iff2 = z80.im = 0;
    z80.halted = 0;
    z80.iff2_read = 0;
    z80.q = 0;

    z80.r16.bc = z80.r16.de = z80.r16.hl = 0;
    z80.r16.bc_ = z80.r16.de_= z80.r16.hl_ = 0;
    z80.r16.ix = z80.r16.iy = 0;
    z80.memptr = 0;

    // z80.interrupts_enabled_at = -1; //??? need some delayed register for that
}


void z80_interrupt()
{


}

void z80_opcocde(*memory_reader: t_memory_reader, memory_writer: t_memory_writer, port_reader: t_port_reader, port_writer: t_port_writer)
{
    uint8 command;
    command = memory_reader(z80.pc);

    if (cb_shifted) {
        switch (command) {
            case 0x00:

            break;
            // ....

            /*
            0x00 RLC B
            0x01 RLC C
            0x02 RLC D
            0x03 RLC E
            0x04 RLC H
            0x05 RLC L
            0x06 RLC (HL)
            0x07 RLC A
            0x08 RRC B
            0x09 RRC C
            0x0a RRC D
            0x0b RRC E
            0x0c RRC H
            0x0d RRC L
            0x0e RRC (HL)
            0x0f RRC A
            0x10 RL B
            0x11 RL C
            0x12 RL D
            0x13 RL E
            0x14 RL H
            0x15 RL L
            0x16 RL (HL)
            0x17 RL A
            0x18 RR B
            0x19 RR C
            0x1a RR D
            0x1b RR E
            0x1c RR H
            0x1d RR L
            0x1e RR (HL)
            0x1f RR A
            0x20 SLA B
            0x21 SLA C
            0x22 SLA D
            0x23 SLA E
            0x24 SLA H
            0x25 SLA L
            0x26 SLA (HL)
            0x27 SLA A
            0x28 SRA B
            0x29 SRA C
            0x2a SRA D
            0x2b SRA E
            0x2c SRA H
            0x2d SRA L
            0x2e SRA (HL)
            0x2f SRA A
            0x30 SLL B
            0x31 SLL C
            0x32 SLL D
            0x33 SLL E
            0x34 SLL H
            0x35 SLL L
            0x36 SLL (HL)
            0x37 SLL A
            0x38 SRL B
            0x39 SRL C
            0x3a SRL D
            0x3b SRL E
            0x3c SRL H
            0x3d SRL L
            0x3e SRL (HL)
            0x3f SRL A
            0x40 BIT 0,B
            0x41 BIT 0,C
            0x42 BIT 0,D
            0x43 BIT 0,E
            0x44 BIT 0,H
            0x45 BIT 0,L
            0x46 BIT 0,(HL)
            0x47 BIT 0,A
            0x48 BIT 1,B
            0x49 BIT 1,C
            0x4a BIT 1,D
            0x4b BIT 1,E
            0x4c BIT 1,H
            0x4d BIT 1,L
            0x4e BIT 1,(HL)
            0x4f BIT 1,A
            0x50 BIT 2,B
            0x51 BIT 2,C
            0x52 BIT 2,D
            0x53 BIT 2,E
            0x54 BIT 2,H
            0x55 BIT 2,L
            0x56 BIT 2,(HL)
            0x57 BIT 2,A
            0x58 BIT 3,B
            0x59 BIT 3,C
            0x5a BIT 3,D
            0x5b BIT 3,E
            0x5c BIT 3,H
            0x5d BIT 3,L
            0x5e BIT 3,(HL)
            0x5f BIT 3,A
            0x60 BIT 4,B
            0x61 BIT 4,C
            0x62 BIT 4,D
            0x63 BIT 4,E
            0x64 BIT 4,H
            0x65 BIT 4,L
            0x66 BIT 4,(HL)
            0x67 BIT 4,A
            0x68 BIT 5,B
            0x69 BIT 5,C
            0x6a BIT 5,D
            0x6b BIT 5,E
            0x6c BIT 5,H
            0x6d BIT 5,L
            0x6e BIT 5,(HL)
            0x6f BIT 5,A
            0x70 BIT 6,B
            0x71 BIT 6,C
            0x72 BIT 6,D
            0x73 BIT 6,E
            0x74 BIT 6,H
            0x75 BIT 6,L
            0x76 BIT 6,(HL)
            0x77 BIT 6,A
            0x78 BIT 7,B
            0x79 BIT 7,C
            0x7a BIT 7,D
            0x7b BIT 7,E
            0x7c BIT 7,H
            0x7d BIT 7,L
            0x7e BIT 7,(HL)
            0x7f BIT 7,A
            0x80 RES 0,B
            0x81 RES 0,C
            0x82 RES 0,D
            0x83 RES 0,E
            0x84 RES 0,H
            0x85 RES 0,L
            0x86 RES 0,(HL)
            0x87 RES 0,A
            0x88 RES 1,B
            0x89 RES 1,C
            0x8a RES 1,D
            0x8b RES 1,E
            0x8c RES 1,H
            0x8d RES 1,L
            0x8e RES 1,(HL)
            0x8f RES 1,A
            0x90 RES 2,B
            0x91 RES 2,C
            0x92 RES 2,D
            0x93 RES 2,E
            0x94 RES 2,H
            0x95 RES 2,L
            0x96 RES 2,(HL)
            0x97 RES 2,A
            0x98 RES 3,B
            0x99 RES 3,C
            0x9a RES 3,D
            0x9b RES 3,E
            0x9c RES 3,H
            0x9d RES 3,L
            0x9e RES 3,(HL)
            0x9f RES 3,A
            0xa0 RES 4,B
            0xa1 RES 4,C
            0xa2 RES 4,D
            0xa3 RES 4,E
            0xa4 RES 4,H
            0xa5 RES 4,L
            0xa6 RES 4,(HL)
            0xa7 RES 4,A
            0xa8 RES 5,B
            0xa9 RES 5,C
            0xaa RES 5,D
            0xab RES 5,E
            0xac RES 5,H
            0xad RES 5,L
            0xae RES 5,(HL)
            0xaf RES 5,A
            0xb0 RES 6,B
            0xb1 RES 6,C
            0xb2 RES 6,D
            0xb3 RES 6,E
            0xb4 RES 6,H
            0xb5 RES 6,L
            0xb6 RES 6,(HL)
            0xb7 RES 6,A
            0xb8 RES 7,B
            0xb9 RES 7,C
            0xba RES 7,D
            0xbb RES 7,E
            0xbc RES 7,H
            0xbd RES 7,L
            0xbe RES 7,(HL)
            0xbf RES 7,A
            0xc0 SET 0,B
            0xc1 SET 0,C
            0xc2 SET 0,D
            0xc3 SET 0,E
            0xc4 SET 0,H
            0xc5 SET 0,L
            0xc6 SET 0,(HL)
            0xc7 SET 0,A
            0xc8 SET 1,B
            0xc9 SET 1,C
            0xca SET 1,D
            0xcb SET 1,E
            0xcc SET 1,H
            0xcd SET 1,L
            0xce SET 1,(HL)
            0xcf SET 1,A
            0xd0 SET 2,B
            0xd1 SET 2,C
            0xd2 SET 2,D
            0xd3 SET 2,E
            0xd4 SET 2,H
            0xd5 SET 2,L
            0xd6 SET 2,(HL)
            0xd7 SET 2,A
            0xd8 SET 3,B
            0xd9 SET 3,C
            0xda SET 3,D
            0xdb SET 3,E
            0xdc SET 3,H
            0xdd SET 3,L
            0xde SET 3,(HL)
            0xdf SET 3,A
            0xe0 SET 4,B
            0xe1 SET 4,C
            0xe2 SET 4,D
            0xe3 SET 4,E
            0xe4 SET 4,H
            0xe5 SET 4,L
            0xe6 SET 4,(HL)
            0xe7 SET 4,A
            0xe8 SET 5,B
            0xe9 SET 5,C
            0xea SET 5,D
            0xeb SET 5,E
            0xec SET 5,H
            0xed SET 5,L
            0xee SET 5,(HL)
            0xef SET 5,A
            0xf0 SET 6,B
            0xf1 SET 6,C
            0xf2 SET 6,D
            0xf3 SET 6,E
            0xf4 SET 6,H
            0xf5 SET 6,L
            0xf6 SET 6,(HL)
            0xf7 SET 6,A
            0xf8 SET 7,B
            0xf9 SET 7,C
            0xfa SET 7,D
            0xfb SET 7,E
            0xfc SET 7,H
            0xfd SET 7,L
            0xfe SET 7,(HL)
            0xff SET 7,A
            */

    }

    switch (command) {
        case 0x00:

        break;
        // ....

        /*
            0x00 NOP
            0x01 LD BC,nnnn
            0x02 LD (BC),A
            0x03 INC BC
            0x04 INC B
            0x05 DEC B
            0x06 LD B,nn
            0x07 RLCA
            0x08 EX AF,AF'
            0x09 ADD HL,BC
            0x0a LD A,(BC)
            0x0b DEC BC
            0x0c INC C
            0x0d DEC C
            0x0e LD C,nn
            0x0f RRCA
            0x10 DJNZ offset
            0x11 LD DE,nnnn
            0x12 LD (DE),A
            0x13 INC DE
            0x14 INC D
            0x15 DEC D
            0x16 LD D,nn
            0x17 RLA
            0x18 JR offset
            0x19 ADD HL,DE
            0x1a LD A,(DE)
            0x1b DEC DE
            0x1c INC E
            0x1d DEC E
            0x1e LD E,nn
            0x1f RRA
            0x20 JR NZ,offset
            0x21 LD HL,nnnn
            0x22 LD (nnnn),HL
            0x23 INC HL
            0x24 INC H
            0x25 DEC H
            0x26 LD H,nn
            0x27 DAA
            0x28 JR Z,offset
            0x29 ADD HL,HL
            0x2a LD HL,(nnnn)
            0x2b DEC HL
            0x2c INC L
            0x2d DEC L
            0x2e LD L,nn
            0x2f CPL
            0x30 JR NC,offset
            0x31 LD SP,nnnn
            0x32 LD (nnnn),A
            0x33 INC SP
            0x34 INC (HL)
            0x35 DEC (HL)
            0x36 LD (HL),nn
            0x37 SCF
            0x38 JR C,offset
            0x39 ADD HL,SP
            0x3a LD A,(nnnn)
            0x3b DEC SP
            0x3c INC A
            0x3d DEC A
            0x3e LD A,nn
            0x3f CCF
            0x40 LD B,B
            0x41 LD B,C
            0x42 LD B,D
            0x43 LD B,E
            0x44 LD B,H
            0x45 LD B,L
            0x46 LD B,(HL)
            0x47 LD B,A
            0x48 LD C,B
            0x49 LD C,C
            0x4a LD C,D
            0x4b LD C,E
            0x4c LD C,H
            0x4d LD C,L
            0x4e LD C,(HL)
            0x4f LD C,A
            0x50 LD D,B
            0x51 LD D,C
            0x52 LD D,D
            0x53 LD D,E
            0x54 LD D,H
            0x55 LD D,L
            0x56 LD D,(HL)
            0x57 LD D,A
            0x58 LD E,B
            0x59 LD E,C
            0x5a LD E,D
            0x5b LD E,E
            0x5c LD E,H
            0x5d LD E,L
            0x5e LD E,(HL)
            0x5f LD E,A
            0x60 LD H,B
            0x61 LD H,C
            0x62 LD H,D
            0x63 LD H,E
            0x64 LD H,H
            0x65 LD H,L
            0x66 LD H,(HL)
            0x67 LD H,A
            0x68 LD L,B
            0x69 LD L,C
            0x6a LD L,D
            0x6b LD L,E
            0x6c LD L,H
            0x6d LD L,L
            0x6e LD L,(HL)
            0x6f LD L,A
            0x70 LD (HL),B
            0x71 LD (HL),C
            0x72 LD (HL),D
            0x73 LD (HL),E
            0x74 LD (HL),H
            0x75 LD (HL),L
            0x76 HALT
            0x77 LD (HL),A
            0x78 LD A,B
            0x79 LD A,C
            0x7a LD A,D
            0x7b LD A,E
            0x7c LD A,H
            0x7d LD A,L
            0x7e LD A,(HL)
            0x7f LD A,A
            0x80 ADD A,B
            0x81 ADD A,C
            0x82 ADD A,D
            0x83 ADD A,E
            0x84 ADD A,H
            0x85 ADD A,L
            0x86 ADD A,(HL)
            0x87 ADD A,A
            0x88 ADC A,B
            0x89 ADC A,C
            0x8a ADC A,D
            0x8b ADC A,E
            0x8c ADC A,H
            0x8d ADC A,L
            0x8e ADC A,(HL)
            0x8f ADC A,A
            0x90 SUB A,B
            0x91 SUB A,C
            0x92 SUB A,D
            0x93 SUB A,E
            0x94 SUB A,H
            0x95 SUB A,L
            0x96 SUB A,(HL)
            0x97 SUB A,A
            0x98 SBC A,B
            0x99 SBC A,C
            0x9a SBC A,D
            0x9b SBC A,E
            0x9c SBC A,H
            0x9d SBC A,L
            0x9e SBC A,(HL)
            0x9f SBC A,A
            0xa0 AND A,B
            0xa1 AND A,C
            0xa2 AND A,D
            0xa3 AND A,E
            0xa4 AND A,H
            0xa5 AND A,L
            0xa6 AND A,(HL)
            0xa7 AND A,A
            0xa8 XOR A,B
            0xa9 XOR A,C
            0xaa XOR A,D
            0xab XOR A,E
            0xac XOR A,H
            0xad XOR A,L
            0xae XOR A,(HL)
            0xaf XOR A,A
            0xb0 OR A,B
            0xb1 OR A,C
            0xb2 OR A,D
            0xb3 OR A,E
            0xb4 OR A,H
            0xb5 OR A,L
            0xb6 OR A,(HL)
            0xb7 OR A,A
            0xb8 CP B
            0xb9 CP C
            0xba CP D
            0xbb CP E
            0xbc CP H
            0xbd CP L
            0xbe CP (HL)
            0xbf CP A
            0xc0 RET NZ
            0xc1 POP BC
            0xc2 JP NZ,nnnn
            0xc3 JP nnnn
            0xc4 CALL NZ,nnnn
            0xc5 PUSH BC
            0xc6 ADD A,nn
            0xc7 RST 00
            0xc8 RET Z
            0xc9 RET
            0xca JP Z,nnnn
            0xcb shift CB
            0xcc CALL Z,nnnn
            0xcd CALL nnnn
            0xce ADC A,nn
            0xcf RST 8
            0xd0 RET NC
            0xd1 POP DE
            0xd2 JP NC,nnnn
            0xd3 OUT (nn),A
            0xd4 CALL NC,nnnn
            0xd5 PUSH DE
            0xd6 SUB nn
            0xd7 RST 10
            0xd8 RET C
            0xd9 EXX
            0xda JP C,nnnn
            0xdb IN A,(nn)
            0xdc CALL C,nnnn
            0xdd shift DD
            0xde SBC A,nn
            0xdf RST 18
            0xe0 RET PO
            0xe1 POP HL
            0xe2 JP PO,nnnn
            0xe3 EX (SP),HL
            0xe4 CALL PO,nnnn
            0xe5 PUSH HL
            0xe6 AND nn
            0xe7 RST 20
            0xe8 RET PE
            0xe9 JP HL
            0xea JP PE,nnnn
            0xeb EX DE,HL
            0xec CALL PE,nnnn
            0xed shift ED
            0xee XOR A,nn
            0xef RST 28
            0xf0 RET P
            0xf1 POP AF
            0xf2 JP P,nnnn
            0xf3 DI
            0xf4 CALL P,nnnn
            0xf5 PUSH AF
            0xf6 OR nn
            0xf7 RST 30
            0xf8 RET M
            0xf9 LD SP,HL
            0xfa JP M,nnnn
            0xfb EI
            0xfc CALL M,nnnn
            0xfd shift FD
            0xfe CP nn
            0xff RST 38
            */
    }
}




//dd/fd

/*
0x19 ADD REGISTER,DE
0x21 LD REGISTER,nnnn
0x22 LD (nnnn),REGISTER
0x23 INC REGISTER
0x24 INC REGISTERH
0x25 DEC REGISTERH
0x26 LD REGISTERH,nn
0x29 ADD REGISTER,REGISTER
0x2a LD REGISTER,(nnnn)
0x2b DEC REGISTER
0x2c INC REGISTERL
0x2d DEC REGISTERL
0x2e LD REGISTERL,nn
0x34 INC (REGISTER+dd)
0x35 DEC (REGISTER+dd)
0x36 LD (REGISTER+dd),nn
0x39 ADD REGISTER,SP
0x44 LD B,REGISTERH
0x45 LD B,REGISTERL
0x46 LD B,(REGISTER+dd)
0x4c LD C,REGISTERH
0x4d LD C,REGISTERL
0x4e LD C,(REGISTER+dd)
0x54 LD D,REGISTERH
0x55 LD D,REGISTERL
0x56 LD D,(REGISTER+dd)
0x5c LD E,REGISTERH
0x5d LD E,REGISTERL
0x5e LD E,(REGISTER+dd)
0x60 LD REGISTERH,B
0x61 LD REGISTERH,C
0x62 LD REGISTERH,D
0x63 LD REGISTERH,E
0x64 LD REGISTERH,REGISTERH
0x65 LD REGISTERH,REGISTERL
0x66 LD H,(REGISTER+dd)
0x67 LD REGISTERH,A
0x68 LD REGISTERL,B
0x69 LD REGISTERL,C
0x6a LD REGISTERL,D
0x6b LD REGISTERL,E
0x6c LD REGISTERL,REGISTERH
0x6d LD REGISTERL,REGISTERL
0x6e LD L,(REGISTER+dd)
0x6f LD REGISTERL,A
0x70 LD (REGISTER+dd),B
0x71 LD (REGISTER+dd),C
0x72 LD (REGISTER+dd),D
0x73 LD (REGISTER+dd),E
0x74 LD (REGISTER+dd),H
0x75 LD (REGISTER+dd),L
0x77 LD (REGISTER+dd),A
0x7c LD A,REGISTERH
0x7d LD A,REGISTERL
0x7e LD A,(REGISTER+dd)
0x84 ADD A,REGISTERH
0x85 ADD A,REGISTERL
0x86 ADD A,(REGISTER+dd)
0x8c ADC A,REGISTERH
0x8d ADC A,REGISTERL
0x8e ADC A,(REGISTER+dd)
0x94 SUB A,REGISTERH
0x95 SUB A,REGISTERL
0x96 SUB A,(REGISTER+dd)
0x9c SBC A,REGISTERH
0x9d SBC A,REGISTERL
0x9e SBC A,(REGISTER+dd)
0xa4 AND A,REGISTERH
0xa5 AND A,REGISTERL
0xa6 AND A,(REGISTER+dd)
0xac XOR A,REGISTERH
0xad XOR A,REGISTERL
0xae XOR A,(REGISTER+dd)
0xb4 OR A,REGISTERH
0xb5 OR A,REGISTERL
0xb6 OR A,(REGISTER+dd)
0xbc CP A,REGISTERH
0xbd CP A,REGISTERL
0xbe CP A,(REGISTER+dd)
0xcb shift DDFDCB
0xe1 POP REGISTER
0xe3 EX (SP),REGISTER
0xe5 PUSH REGISTER
0xe9 JP REGISTER
0xf9 LD SP,REGISTER

*/


// dd/fd cb
//0x00 LD B,RLC (REGISTER+dd)

/*
0x01 LD C,RLC (REGISTER+dd)
0x02 LD D,RLC (REGISTER+dd)
0x03 LD E,RLC (REGISTER+dd)
0x04 LD H,RLC (REGISTER+dd)
0x05 LD L,RLC (REGISTER+dd)
0x06 RLC (REGISTER+dd)
0x07 LD A,RLC (REGISTER+dd)
0x08 LD B,RRC (REGISTER+dd)
0x09 LD C,RRC (REGISTER+dd)
0x0a LD D,RRC (REGISTER+dd)
0x0b LD E,RRC (REGISTER+dd)
0x0c LD H,RRC (REGISTER+dd)
0x0d LD L,RRC (REGISTER+dd)
0x0e RRC (REGISTER+dd)
0x0f LD A,RRC (REGISTER+dd)
0x10 LD B,RL (REGISTER+dd)
0x11 LD C,RL (REGISTER+dd)
0x12 LD D,RL (REGISTER+dd)
0x13 LD E,RL (REGISTER+dd)
0x14 LD H,RL (REGISTER+dd)
0x15 LD L,RL (REGISTER+dd)
0x16 RL (REGISTER+dd)
0x17 LD A,RL (REGISTER+dd)
0x18 LD B,RR (REGISTER+dd)
0x19 LD C,RR (REGISTER+dd)
0x1a LD D,RR (REGISTER+dd)
0x1b LD E,RR (REGISTER+dd)
0x1c LD H,RR (REGISTER+dd)
0x1d LD L,RR (REGISTER+dd)
0x1e RR (REGISTER+dd)
0x1f LD A,RR (REGISTER+dd)
0x20 LD B,SLA (REGISTER+dd)
0x21 LD C,SLA (REGISTER+dd)
0x22 LD D,SLA (REGISTER+dd)
0x23 LD E,SLA (REGISTER+dd)
0x24 LD H,SLA (REGISTER+dd)
0x25 LD L,SLA (REGISTER+dd)
0x26 SLA (REGISTER+dd)
0x27 LD A,SLA (REGISTER+dd)
0x28 LD B,SRA (REGISTER+dd)
0x29 LD C,SRA (REGISTER+dd)
0x2a LD D,SRA (REGISTER+dd)
0x2b LD E,SRA (REGISTER+dd)
0x2c LD H,SRA (REGISTER+dd)
0x2d LD L,SRA (REGISTER+dd)
0x2e SRA (REGISTER+dd)
0x2f LD A,SRA (REGISTER+dd)
0x30 LD B,SLL (REGISTER+dd)
0x31 LD C,SLL (REGISTER+dd)
0x32 LD D,SLL (REGISTER+dd)
0x33 LD E,SLL (REGISTER+dd)
0x34 LD H,SLL (REGISTER+dd)
0x35 LD L,SLL (REGISTER+dd)
0x36 SLL (REGISTER+dd)
0x37 LD A,SLL (REGISTER+dd)
0x38 LD B,SRL (REGISTER+dd)
0x39 LD C,SRL (REGISTER+dd)
0x3a LD D,SRL (REGISTER+dd)
0x3b LD E,SRL (REGISTER+dd)
0x3c LD H,SRL (REGISTER+dd)
0x3d LD L,SRL (REGISTER+dd)
0x3e SRL (REGISTER+dd)
0x3f LD A,SRL (REGISTER+dd)
0x40
0x41
0x42
0x43
0x44
0x45
0x46
0x47 BIT 0,(REGISTER+dd)
0x48
0x49
0x4a
0x4b
0x4c
0x4d
0x4e
0x4f BIT 1,(REGISTER+dd)
0x50
0x51
0x52
0x53
0x54
0x55
0x56
0x57 BIT 2,(REGISTER+dd)
0x58
0x59
0x5a
0x5b
0x5c
0x5d
0x5e
0x5f BIT 3,(REGISTER+dd)
0x60
0x61
0x62
0x63
0x64
0x65
0x66
0x67 BIT 4,(REGISTER+dd)
0x68
0x69
0x6a
0x6b
0x6c
0x6d
0x6e
0x6f BIT 5,(REGISTER+dd)
0x70
0x71
0x72
0x73
0x74
0x75
0x76
0x77 BIT 6,(REGISTER+dd)
0x78
0x79
0x7a
0x7b
0x7c
0x7d
0x7e
0x7f BIT 7,(REGISTER+dd)
0x80 LD B,RES 0,(REGISTER+dd)
0x81 LD C,RES 0,(REGISTER+dd)
0x82 LD D,RES 0,(REGISTER+dd)
0x83 LD E,RES 0,(REGISTER+dd)
0x84 LD H,RES 0,(REGISTER+dd)
0x85 LD L,RES 0,(REGISTER+dd)
0x86 RES 0,(REGISTER+dd)
0x87 LD A,RES 0,(REGISTER+dd)
0x88 LD B,RES 1,(REGISTER+dd)
0x89 LD C,RES 1,(REGISTER+dd)
0x8a LD D,RES 1,(REGISTER+dd)
0x8b LD E,RES 1,(REGISTER+dd)
0x8c LD H,RES 1,(REGISTER+dd)
0x8d LD L,RES 1,(REGISTER+dd)
0x8e RES 1,(REGISTER+dd)
0x8f LD A,RES 1,(REGISTER+dd)
0x90 LD B,RES 2,(REGISTER+dd)
0x91 LD C,RES 2,(REGISTER+dd)
0x92 LD D,RES 2,(REGISTER+dd)
0x93 LD E,RES 2,(REGISTER+dd)
0x94 LD H,RES 2,(REGISTER+dd)
0x95 LD L,RES 2,(REGISTER+dd)
0x96 RES 2,(REGISTER+dd)
0x97 LD A,RES 2,(REGISTER+dd)
0x98 LD B,RES 3,(REGISTER+dd)
0x99 LD C,RES 3,(REGISTER+dd)
0x9a LD D,RES 3,(REGISTER+dd)
0x9b LD E,RES 3,(REGISTER+dd)
0x9c LD H,RES 3,(REGISTER+dd)
0x9d LD L,RES 3,(REGISTER+dd)
0x9e RES 3,(REGISTER+dd)
0x9f LD A,RES 3,(REGISTER+dd)
0xa0 LD B,RES 4,(REGISTER+dd)
0xa1 LD C,RES 4,(REGISTER+dd)
0xa2 LD D,RES 4,(REGISTER+dd)
0xa3 LD E,RES 4,(REGISTER+dd)
0xa4 LD H,RES 4,(REGISTER+dd)
0xa5 LD L,RES 4,(REGISTER+dd)
0xa6 RES 4,(REGISTER+dd)
0xa7 LD A,RES 4,(REGISTER+dd)
0xa8 LD B,RES 5,(REGISTER+dd)
0xa9 LD C,RES 5,(REGISTER+dd)
0xaa LD D,RES 5,(REGISTER+dd)
0xab LD E,RES 5,(REGISTER+dd)
0xac LD H,RES 5,(REGISTER+dd)
0xad LD L,RES 5,(REGISTER+dd)
0xae RES 5,(REGISTER+dd)
0xaf LD A,RES 5,(REGISTER+dd)
0xb0 LD B,RES 6,(REGISTER+dd)
0xb1 LD C,RES 6,(REGISTER+dd)
0xb2 LD D,RES 6,(REGISTER+dd)
0xb3 LD E,RES 6,(REGISTER+dd)
0xb4 LD H,RES 6,(REGISTER+dd)
0xb5 LD L,RES 6,(REGISTER+dd)
0xb6 RES 6,(REGISTER+dd)
0xb7 LD A,RES 6,(REGISTER+dd)
0xb8 LD B,RES 7,(REGISTER+dd)
0xb9 LD C,RES 7,(REGISTER+dd)
0xba LD D,RES 7,(REGISTER+dd)
0xbb LD E,RES 7,(REGISTER+dd)
0xbc LD H,RES 7,(REGISTER+dd)
0xbd LD L,RES 7,(REGISTER+dd)
0xbe RES 7,(REGISTER+dd)
0xbf LD A,RES 7,(REGISTER+dd)
0xc0 LD B,SET 0,(REGISTER+dd)
0xc1 LD C,SET 0,(REGISTER+dd)
0xc2 LD D,SET 0,(REGISTER+dd)
0xc3 LD E,SET 0,(REGISTER+dd)
0xc4 LD H,SET 0,(REGISTER+dd)
0xc5 LD L,SET 0,(REGISTER+dd)
0xc6 SET 0,(REGISTER+dd)
0xc7 LD A,SET 0,(REGISTER+dd)
0xc8 LD B,SET 1,(REGISTER+dd)
0xc9 LD C,SET 1,(REGISTER+dd)
0xca LD D,SET 1,(REGISTER+dd)
0xcb LD E,SET 1,(REGISTER+dd)
0xcc LD H,SET 1,(REGISTER+dd)
0xcd LD L,SET 1,(REGISTER+dd)
0xce SET 1,(REGISTER+dd)
0xcf LD A,SET 1,(REGISTER+dd)
0xd0 LD B,SET 2,(REGISTER+dd)
0xd1 LD C,SET 2,(REGISTER+dd)
0xd2 LD D,SET 2,(REGISTER+dd)
0xd3 LD E,SET 2,(REGISTER+dd)
0xd4 LD H,SET 2,(REGISTER+dd)
0xd5 LD L,SET 2,(REGISTER+dd)
0xd6 SET 2,(REGISTER+dd)
0xd7 LD A,SET 2,(REGISTER+dd)
0xd8 LD B,SET 3,(REGISTER+dd)
0xd9 LD C,SET 3,(REGISTER+dd)
0xda LD D,SET 3,(REGISTER+dd)
0xdb LD E,SET 3,(REGISTER+dd)
0xdc LD H,SET 3,(REGISTER+dd)
0xdd LD L,SET 3,(REGISTER+dd)
0xde SET 3,(REGISTER+dd)
0xdf LD A,SET 3,(REGISTER+dd)
0xe0 LD B,SET 4,(REGISTER+dd)
0xe1 LD C,SET 4,(REGISTER+dd)
0xe2 LD D,SET 4,(REGISTER+dd)
0xe3 LD E,SET 4,(REGISTER+dd)
0xe4 LD H,SET 4,(REGISTER+dd)
0xe5 LD L,SET 4,(REGISTER+dd)
0xe6 SET 4,(REGISTER+dd)
0xe7 LD A,SET 4,(REGISTER+dd)
0xe8 LD B,SET 5,(REGISTER+dd)
0xe9 LD C,SET 5,(REGISTER+dd)
0xea LD D,SET 5,(REGISTER+dd)
0xeb LD E,SET 5,(REGISTER+dd)
0xec LD H,SET 5,(REGISTER+dd)
0xed LD L,SET 5,(REGISTER+dd)
0xee SET 5,(REGISTER+dd)
0xef LD A,SET 5,(REGISTER+dd)
0xf0 LD B,SET 6,(REGISTER+dd)
0xf1 LD C,SET 6,(REGISTER+dd)
0xf2 LD D,SET 6,(REGISTER+dd)
0xf3 LD E,SET 6,(REGISTER+dd)
0xf4 LD H,SET 6,(REGISTER+dd)
0xf5 LD L,SET 6,(REGISTER+dd)
0xf6 SET 6,(REGISTER+dd)
0xf7 LD A,SET 6,(REGISTER+dd)
0xf8 LD B,SET 7,(REGISTER+dd)
0xf9 LD C,SET 7,(REGISTER+dd)
0xfa LD D,SET 7,(REGISTER+dd)
0xfb LD E,SET 7,(REGISTER+dd)
0xfc LD H,SET 7,(REGISTER+dd)
0xfd LD L,SET 7,(REGISTER+dd)
0xfe SET 7,(REGISTER+dd)
0xff LD A,SET 7,(REGISTER+dd)

*/


// ed
/*
0x40 IN B,(C)
0x41 OUT (C),B
0x42 SBC HL,BC
0x43 LD (nnnn),BC
0x44
0x4c
0x54
0x5c
0x64
0x6c
0x74
0x7c NEG
0x45
0x4d
0x55
0x5d
0x65
0x6d
0x75
0x7d RETN
0x46
0x4e
0x66
0x6e IM 0
0x47 LD I,A
0x48 IN C,(C)
0x49 OUT (C),C
0x4a ADC HL,BC
0x4b LD BC,(nnnn)
0x4f LD R,A
0x50 IN D,(C)
0x51 OUT (C),D
0x52 SBC HL,DE
0x53 LD (nnnn),DE
0x56
0x76 IM 1
0x57 LD A,I
0x58 IN E,(C)
0x59 OUT (C),E
0x5a ADC HL,DE
0x5b LD DE,(nnnn)
0x5e
0x7e IM 2
0x5f LD A,R
0x60 IN H,(C)
0x61 OUT (C),H
0x62 SBC HL,HL
0x63 LD (nnnn),HL
0x67 RRD
0x68 IN L,(C)
0x69 OUT (C),L
0x6a ADC HL,HL
0x6b LD HL,(nnnn)
0x6f RLD
0x70 IN F,(C)
0x71 OUT (C),0
0x72 SBC HL,SP
0x73 LD (nnnn),SP
0x78 IN A,(C)
0x79 OUT (C),A
0x7a ADC HL,SP
0x7b LD SP,(nnnn)
0xa0 LDI
0xa1 CPI
0xa2 INI
0xa3 OUTI
0xa8 LDD
0xa9 CPD
0xaa IND
0xab OUTD
0xb0 LDIR
0xb1 CPIR
0xb2 INIR
0xb3 OTIR
0xb8 LDDR
0xb9 CPDR
0xba INDR
0xbb OTDR
0xfb slttrap


*/



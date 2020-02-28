#include <stddef.h>
#include <stdio.h>
#include "../z80.h"


//
typedef enum {
    ST_ANY,
    ST_B1,
    ST_B0,
    ST_BC0,
    ST_BC1,
    ST_C,
    ST_NC,
    ST_Z,
    ST_NZ,
    ST_PO,
    ST_PE,
    ST_P,
    ST_M
} precond;


struct test_unit
{
    uint8_t opcode;
    char *name;
    uint8_t states;
    precond pc_type;
};


uint8_t memory_reader(uint16_t address) {
    return 0x00;
}


void memory_writer(uint16_t address, uint8_t byte) {
    return;
}


uint8_t port_reader(uint16_t address) {
    return 0x00;
}


void port_writer(uint16_t address, uint8_t byte) {
    return;
}


struct test_unit base_opcodes[] = {

    {0x00, "NOP",         4, ST_ANY },
    {0x01, "LD BC,@",    10, ST_ANY },
    {0x02, "LD (BC),A",   7, ST_ANY },
    {0x03, "INC BC",      6, ST_ANY },
    {0x04, "INC B",       4, ST_ANY },
    {0x05, "DEC B",       4, ST_ANY },
    {0x06, "LD B,#",      7, ST_ANY },
    {0x07, "RLCA",        4, ST_ANY },
    {0x08, "EX AF,AF'",   4, ST_ANY },
    {0x09, "ADD HL,BC",  11, ST_ANY },
    {0x0a, "LD A,(BC)",   7, ST_ANY },
    {0x0b, "DEC BC",      6, ST_ANY },
    {0x0c, "INC C",       4, ST_ANY },
    {0x0d, "DEC C",       4, ST_ANY },
    {0x0e, "LD C,#",      7, ST_ANY },
    {0x0f, "RRCA",        4, ST_ANY },
    {0x10, "DJNZ %",      8, ST_B1  },
    {0x10, "DJNZ %",     13, ST_B0  },
    {0x11, "LD DE,@",    10, ST_ANY },
    {0x12, "LD (DE),A",   7, ST_ANY },
    {0x13, "INC DE",      6, ST_ANY },
    {0x14, "INC D",       4, ST_ANY },
    {0x15, "DEC D",       4, ST_ANY },
    {0x16, "LD D,#",      7, ST_ANY },
    {0x17, "RLA",         4, ST_ANY },
    {0x18, "JR %",       12, ST_ANY },
    {0x19, "ADD HL,DE",  11, ST_ANY },
    {0x1a, "LD A,(DE)",   7, ST_ANY },
    {0x1b, "DEC DE",      6, ST_ANY },
    {0x1c, "INC E",       4, ST_ANY },
    {0x1d, "DEC E",       4, ST_ANY },
    {0x1e, "LD E,#",      7, ST_ANY },
    {0x1f, "RRA",         4, ST_ANY },
    {0x20, "JR NZ,%",    12, ST_NZ  },
    {0x20, "JR NZ,%",     7, ST_Z   },
    {0x21, "LD HL,@",    10, ST_ANY },
    {0x22, "LD (@),HL",  16, ST_ANY },
    {0x23, "INC HL",      6, ST_ANY },
    {0x24, "INC H",       4, ST_ANY },
    {0x25, "DEC H",       4, ST_ANY },
    {0x26, "LD H,#",      7, ST_ANY },
    {0x27, "DAA",         4, ST_ANY },
    {0x28, "JR Z,%",     12, ST_Z   },
    {0x28, "JR Z,%",      7, ST_NZ  },
    {0x29, "ADD HL,HL",  11, ST_ANY },
    {0x2a, "LD HL,(@)",  16, ST_ANY },
    {0x2b, "DEC HL",      6, ST_ANY },
    {0x2c, "INC L",       4, ST_ANY },
    {0x2d, "DEC L",       4, ST_ANY },
    {0x2e, "LD L,#",      7, ST_ANY },
    {0x2f, "CPL",         4, ST_ANY },
    {0x30, "JR NC,%",    12, ST_NC  },
    {0x30, "JR NC,%",     7, ST_C   },
    {0x31, "LD SP,@",    10, ST_ANY },
    {0x32, "LD (@),A",   13, ST_ANY },
    {0x33, "INC SP",      6, ST_ANY },
    {0x34, "INC (HL)",   11, ST_ANY },
    {0x35, "DEC (HL)",   11, ST_ANY },
    {0x36, "LD (HL),#",  10, ST_ANY },
    {0x37, "SCF",         4, ST_ANY },
    {0x38, "JR C,%",     12, ST_C   },
    {0x38, "JR C,%",      7, ST_NC  },
    {0x39, "ADD HL,SP",  11, ST_ANY },
    {0x3a, "LD A,(@)",   13, ST_ANY },
    {0x3b, "DEC SP",      6, ST_ANY },
    {0x3c, "INC A",       4, ST_ANY },
    {0x3d, "DEC A",       4, ST_ANY },
    {0x3e, "LD A,#",      7, ST_ANY },
    {0x3f, "CCF",         4, ST_ANY },
    {0x40, "LD B,B",      4, ST_ANY },
    {0x41, "LD B,C",      4, ST_ANY },
    {0x42, "LD B,D",      4, ST_ANY },
    {0x43, "LD B,E",      4, ST_ANY },
    {0x44, "LD B,H",      4, ST_ANY },
    {0x45, "LD B,L",      4, ST_ANY },
    {0x46, "LD B,(HL)",   7, ST_ANY },
    {0x47, "LD B,A",      4, ST_ANY },
    {0x48, "LD C,B",      4, ST_ANY },
    {0x49, "LD C,C",      4, ST_ANY },
    {0x4a, "LD C,D",      4, ST_ANY },
    {0x4b, "LD C,E",      4, ST_ANY },
    {0x4c, "LD C,H",      4, ST_ANY },
    {0x4d, "LD C,L",      4, ST_ANY },
    {0x4e, "LD C,(HL)",   7, ST_ANY },
    {0x4f, "LD C,A",      4, ST_ANY },
    {0x50, "LD D,B",      4, ST_ANY },
    {0x51, "LD D,C",      4, ST_ANY },
    {0x52, "LD D,D",      4, ST_ANY },
    {0x53, "LD D,E",      4, ST_ANY },
    {0x54, "LD D,H",      4, ST_ANY },
    {0x55, "LD D,L",      4, ST_ANY },
    {0x56, "LD D,(HL)",   7, ST_ANY },
    {0x57, "LD D,A",      4, ST_ANY },
    {0x58, "LD E,B",      4, ST_ANY },
    {0x59, "LD E,C",      4, ST_ANY },
    {0x5a, "LD E,D",      4, ST_ANY },
    {0x5b, "LD E,E",      4, ST_ANY },
    {0x5c, "LD E,H",      4, ST_ANY },
    {0x5d, "LD E,L",      4, ST_ANY },
    {0x5e, "LD E,(HL)",   7, ST_ANY },
    {0x5f, "LD E,A",      4, ST_ANY },
    {0x60, "LD H,B",      4, ST_ANY },
    {0x61, "LD H,C",      4, ST_ANY },
    {0x62, "LD H,D",      4, ST_ANY },
    {0x63, "LD H,E",      4, ST_ANY },
    {0x64, "LD H,H",      4, ST_ANY },
    {0x65, "LD H,L",      4, ST_ANY },
    {0x66, "LD H,(HL)",   7, ST_ANY },
    {0x67, "LD H,A",      4, ST_ANY },
    {0x68, "LD L,B",      4, ST_ANY },
    {0x69, "LD L,C",      4, ST_ANY },
    {0x6a, "LD L,D",      4, ST_ANY },
    {0x6b, "LD L,E",      4, ST_ANY },
    {0x6c, "LD L,H",      4, ST_ANY },
    {0x6d, "LD L,L",      4, ST_ANY },
    {0x6e, "LD L,(HL)",   7, ST_ANY },
    {0x6f, "LD L,A",      4, ST_ANY },
    {0x70, "LD (HL),B",   7, ST_ANY },
    {0x71, "LD (HL),C",   7, ST_ANY },
    {0x72, "LD (HL),D",   7, ST_ANY },
    {0x73, "LD (HL),E",   7, ST_ANY },
    {0x74, "LD (HL),H",   7, ST_ANY },
    {0x75, "LD (HL),L",   7, ST_ANY },
    {0x76, "HALT",        4, ST_ANY },
    {0x77, "LD (HL),A",   7, ST_ANY },
    {0x78, "LD A,B",      4, ST_ANY },
    {0x79, "LD A,C",      4, ST_ANY },
    {0x7a, "LD A,D",      4, ST_ANY },
    {0x7b, "LD A,E",      4, ST_ANY },
    {0x7c, "LD A,H",      4, ST_ANY },
    {0x7d, "LD A,L",      4, ST_ANY },
    {0x7e, "LD A,(HL)",   7, ST_ANY },
    {0x7f, "LD A,A",      4, ST_ANY },
    {0x80, "ADD A,B",     4, ST_ANY },
    {0x81, "ADD A,C",     4, ST_ANY },
    {0x82, "ADD A,D",     4, ST_ANY },
    {0x83, "ADD A,E",     4, ST_ANY },
    {0x84, "ADD A,H",     4, ST_ANY },
    {0x85, "ADD A,L",     4, ST_ANY },
    {0x86, "ADD A,(HL)",  7, ST_ANY },
    {0x87, "ADD A,A",     4, ST_ANY },
    {0x88, "ADC A,B",     4, ST_ANY },
    {0x89, "ADC A,C",     4, ST_ANY },
    {0x8a, "ADC A,D",     4, ST_ANY },
    {0x8b, "ADC A,E",     4, ST_ANY },
    {0x8c, "ADC A,H",     4, ST_ANY },
    {0x8d, "ADC A,L",     4, ST_ANY },
    {0x8e, "ADC A,(HL)",  7, ST_ANY },
    {0x8f, "ADC A,A",     4, ST_ANY },
    {0x90, "SUB B",       4, ST_ANY },
    {0x91, "SUB C",       4, ST_ANY },
    {0x92, "SUB D",       4, ST_ANY },
    {0x93, "SUB E",       4, ST_ANY },
    {0x94, "SUB H",       4, ST_ANY },
    {0x95, "SUB L",       4, ST_ANY },
    {0x96, "SUB (HL)",    7, ST_ANY },
    {0x97, "SUB A",       4, ST_ANY },
    {0x98, "SBC A,B",     4, ST_ANY },
    {0x99, "SBC A,C",     4, ST_ANY },
    {0x9a, "SBC A,D",     4, ST_ANY },
    {0x9b, "SBC A,E",     4, ST_ANY },
    {0x9c, "SBC A,H",     4, ST_ANY },
    {0x9d, "SBC A,L",     4, ST_ANY },
    {0x9e, "SBC A,(HL)",  7, ST_ANY },
    {0x9f, "SBC A,A",     4, ST_ANY },
    {0xa0, "AND B",       4, ST_ANY },
    {0xa1, "AND C",       4, ST_ANY },
    {0xa2, "AND D",       4, ST_ANY },
    {0xa3, "AND E",       4, ST_ANY },
    {0xa4, "AND H",       4, ST_ANY },
    {0xa5, "AND L",       4, ST_ANY },
    {0xa6, "AND (HL)",    7, ST_ANY },
    {0xa7, "AND A",       4, ST_ANY },
    {0xa8, "XOR B",       4, ST_ANY },
    {0xa9, "XOR C",       4, ST_ANY },
    {0xaa, "XOR D",       4, ST_ANY },
    {0xab, "XOR E",       4, ST_ANY },
    {0xac, "XOR H",       4, ST_ANY },
    {0xad, "XOR L",       4, ST_ANY },
    {0xae, "XOR (HL)",    7, ST_ANY },
    {0xaf, "XOR A",       4, ST_ANY },
    {0xb0, "OR B",        4, ST_ANY },
    {0xb1, "OR C",        4, ST_ANY },
    {0xb2, "OR D",        4, ST_ANY },
    {0xb3, "OR E",        4, ST_ANY },
    {0xb4, "OR H",        4, ST_ANY },
    {0xb5, "OR L",        4, ST_ANY },
    {0xb6, "OR (HL)",     7, ST_ANY },
    {0xb7, "OR A",        4, ST_ANY },
    {0xb8, "CP B",        4, ST_ANY },
    {0xb9, "CP C",        4, ST_ANY },
    {0xba, "CP D",        4, ST_ANY },
    {0xbb, "CP E",        4, ST_ANY },
    {0xbc, "CP H",        4, ST_ANY },
    {0xbd, "CP L",        4, ST_ANY },
    {0xbe, "CP (HL)",     7, ST_ANY },
    {0xbf, "CP A",        4, ST_ANY },
    {0xc0, "RET NZ",     11, ST_NZ  },
    {0xc0, "RET NZ",      5, ST_Z   },
    {0xc1, "POP BC",     10, ST_ANY },
    {0xc2, "JP NZ,@",    10, ST_NZ  },
    {0xc2, "JP NZ,@",    10, ST_Z   },
    {0xc3, "JP @",       10, ST_ANY },
    {0xc4, "CALL NZ,@",  17, ST_NZ  },
    {0xc4, "CALL NZ,@",  10, ST_Z   },
    {0xc5, "PUSH BC",    11, ST_ANY },
    {0xc6, "ADD A,#",     7, ST_ANY },
    {0xc7, "RST 0x00",   11, ST_ANY },
    {0xc8, "RET Z",      11, ST_Z   },
    {0xc8, "RET Z",       5, ST_NZ  },
    {0xc9, "RET",        10, ST_ANY },
    {0xca, "JP Z,@",     10, ST_Z   },
    {0xca, "JP Z,@",     10, ST_NZ  },
    //{0xcb, "shift CB",    4, ST_ANY },
    {0xcc, "CALL Z,@",   17, ST_Z   },
    {0xcc, "CALL Z,@",   10, ST_NZ  },
    {0xcd, "CALL @",     17, ST_ANY },
    {0xce, "ADC A,#",     7, ST_ANY },
    {0xcf, "RST 0x08",   11, ST_ANY },
    {0xd0, "RET NC",     11, ST_NC  },
    {0xd0, "RET NC",      5, ST_C   },
    {0xd1, "POP DE",     10, ST_ANY },
    {0xd2, "JP NC,@",    10, ST_NC  },
    {0xd2, "JP NC,@",    10, ST_C   },
    {0xd3, "OUT (#),A",  11, ST_ANY },
    {0xd4, "CALL NC,@",  17, ST_NC  },
    {0xd4, "CALL NC,@",  10, ST_C   },
    {0xd5, "PUSH DE",    11, ST_ANY },
    {0xd6, "SUB #",       7, ST_ANY },
    {0xd7, "RST 0x10",   11, ST_ANY },
    {0xd8, "RET C",      11, ST_C   },
    {0xd8, "RET C",       5, ST_NC  },
    {0xd9, "EXX",         4, ST_ANY },
    {0xda, "JP C,@",     10, ST_C   },
    {0xda, "JP C,@",     10, ST_NC  },
    {0xdb, "IN A,(#)",   11, ST_ANY },
    {0xdc, "CALL C,@",   17, ST_C   },
    {0xdc, "CALL C,@",   10, ST_NC  },
    //{0xdd, "shift DD",    4, ST_ANY },
    {0xde, "SBC A,#",     7, ST_ANY },
    {0xdf, "RST 0x18",   11, ST_ANY },
    {0xe0, "RET PO",     11, ST_PO  },
    {0xe0, "RET PO",      5, ST_PE  },
    {0xe1, "POP HL",     10, ST_ANY },
    {0xe2, "JP PO,@",    10, ST_PO  },
    {0xe2, "JP PO,@",    10, ST_PE  },
    {0xe3, "EX (SP),HL", 19, ST_ANY },
    {0xe4, "CALL PO,@",  17, ST_PO  },
    {0xe4, "CALL PO,@",  10, ST_PE  },
    {0xe5, "PUSH HL",    11, ST_ANY },
    {0xe6, "AND #",       7, ST_ANY },
    {0xe7, "RST 0x20",   11, ST_ANY },
    {0xe8, "RET PE",     11, ST_PE  },
    {0xe8, "RET PE",      5, ST_PO  },
    {0xe9, "JP HL",       4, ST_ANY },
    {0xea, "JP PE,@",    10, ST_PE  },
    {0xea, "JP PE,@",    10, ST_PO  },
    {0xeb, "EX DE,HL",    4, ST_ANY },
    {0xec, "CALL PE,@",  17, ST_PE  },
    {0xec, "CALL PE,@",  10, ST_PO  },
    //{0xed, "shift ED",    4, ST_ANY },
    {0xee, "XOR #",       7, ST_ANY },
    {0xef, "RST 0x28",   11, ST_ANY },
    {0xf0, "RET P",      11, ST_P   },
    {0xf0, "RET P",       5, ST_M   },
    {0xf1, "POP AF",     10, ST_ANY },
    {0xf2, "JP P,@",     10, ST_P   },
    {0xf2, "JP P,@",     10, ST_M   },
    {0xf3, "DI",          4, ST_ANY },
    {0xf4, "CALL P,@",   17, ST_P   },
    {0xf4, "CALL P,@",   10, ST_M   },
    {0xf5, "PUSH AF",    11, ST_ANY },
    {0xf6, "OR #",        7, ST_ANY },
    {0xf7, "RST 0x30",   11, ST_ANY },
    {0xf8, "RET M",      11, ST_M   },
    {0xf8, "RET M",       5, ST_P   },
    {0xf9, "LD SP,HL",    6, ST_ANY },
    {0xfa, "JP M,@",     10, ST_M   },
    {0xfa, "JP M,@",     10, ST_P   },
    {0xfb, "EI",          4, ST_ANY },
    {0xfc, "CALL M,@",   17, ST_M   },
    {0xfc, "CALL M,@",   10, ST_P   },
    //{0xfd, "shift FD",    4, ST_ANY },
    {0xfe, "CP #",        7, ST_ANY },
    {0xff, "RST 0x38",   11, ST_ANY },
    {0, "", 0, ST_ANY} // dummy item for end of array
};



int main() {

    printf("Hello, World!\n");

    z80_init(&memory_reader, &memory_writer, &port_reader, &port_writer);

    int i = 0;

    do {
        // base_opcodes
        printf("i = %d\n", i);

    } while (i<100);

    return 0;
}



/*
ddfd


0x09="ADD REGISTER,BC"             , 15"
0x19="ADD REGISTER,DE"             , 15"
0x21="LD REGISTER,@"            , 14"
0x22="LD (@),REGISTER"          , 20"   wr="14,17"
0x23="INC REGISTER"                , 10"
0x24="INC REGISTERH"               , 8"
0x25="DEC REGISTERH"               , 8"
0x26="LD REGISTERH,#"             , 11"
0x29="ADD REGISTER,REGISTER"       , 15"
0x2a="LD REGISTER,(@)"          , 20"   rd="14,17"
0x2b="DEC REGISTER"                , 10"
0x2c="INC REGISTERL"               , 8"
0x2d="DEC REGISTERL"               , 8"
0x2e="LD REGISTERL,#"             , 11"
0x34="INC (REGISTER+$)"           , 23"   rd="16"   wr="20"
0x35="DEC (REGISTER+$)"           , 23"   rd="16"   wr="20"
0x36="LD (REGISTER+$),#"         , 19"   wr="16"
0x39="ADD REGISTER,SP"             , 15"
0x44="LD B,REGISTERH"              , 8"
0x45="LD B,REGISTERL"              , 8"
0x46="LD B,(REGISTER+$)"          , 19"   rd="16"
0x4c="LD C,REGISTERH"              , 8"
0x4d="LD C,REGISTERL"              , 8"
0x4e="LD C,(REGISTER+$)"          , 19"   rd="16"
0x54="LD D,REGISTERH"              , 8"
0x55="LD D,REGISTERL"              , 8"
0x56="LD D,(REGISTER+$)"          , 19"   rd="16"
0x5c="LD E,REGISTERH"              , 8"
0x5d="LD E,REGISTERL"              , 8"
0x5e="LD E,(REGISTER+$)"          , 19"   rd="16"
0x60="LD REGISTERH,B"              , 8"
0x61="LD REGISTERH,C"              , 8"
0x62="LD REGISTERH,D"              , 8"
0x63="LD REGISTERH,E"              , 8"
0x64="LD REGISTERH,REGISTERH"      , 8"
0x65="LD REGISTERH,REGISTERL"      , 8"
0x66="LD H,(REGISTER+$)"          , 19"   rd="16"
0x67="LD REGISTERH,A"              , 8"
0x68="LD REGISTERL,B"              , 8"
0x69="LD REGISTERL,C"              , 8"
0x6a="LD REGISTERL,D"              , 8"
0x6b="LD REGISTERL,E"              , 8"
0x6c="LD REGISTERL,REGISTERH"      , 8"
0x6d="LD REGISTERL,REGISTERL"      , 8"
0x6e="LD L,(REGISTER+$)"          , 19"   rd="16"
0x6f="LD REGISTERL,A"              , 8"
0x70="LD (REGISTER+$),B"          , 19"   wr="16"
0x71="LD (REGISTER+$),C"          , 19"   wr="16"
0x72="LD (REGISTER+$),D"          , 19"   wr="16"
0x73="LD (REGISTER+$),E"          , 19"   wr="16"
0x74="LD (REGISTER+$),H"          , 19"   wr="16"
0x75="LD (REGISTER+$),L"          , 19"   wr="16"
0x77="LD (REGISTER+$),A"          , 19"   wr="16"
0x7c="LD A,REGISTERH"              , 8"
0x7d="LD A,REGISTERL"              , 8"
0x7e="LD A,(REGISTER+$)"          , 19"   rd="16"
0x84="ADD A,REGISTERH"             , 8"
0x85="ADD A,REGISTERL"             , 8"
0x86="ADD A,(REGISTER+$)"         , 19"   rd="16"
0x8c="ADC A,REGISTERH"             , 8"
0x8d="ADC A,REGISTERL"             , 8"
0x8e="ADC A,(REGISTER+$)"         , 19"   rd="16"
0x94="SUB REGISTERH"             , 8"
0x95="SUB REGISTERL"             , 8"
0x96="SUB (REGISTER+$)"         , 19"   rd="16"
0x9c="SBC A,REGISTERH"             , 8"
0x9d="SBC A,REGISTERL"             , 8"
0x9e="SBC A,(REGISTER+$)"         , 19"   rd="16"
0xa4="AND REGISTERH"             , 8"
0xa5="AND REGISTERL"             , 8"
0xa6="AND (REGISTER+$)"         , 19"   rd="16"
0xac="XOR REGISTERH"             , 8"
0xad="XOR REGISTERL"             , 8"
0xae="XOR (REGISTER+$)"         , 19"   rd="16"
0xb4="OR REGISTERH"              , 8"
0xb5="OR REGISTERL"              , 8"
0xb6="OR (REGISTER+$)"          , 19"   rd="16"
0xbc="CP REGISTERH"              , 8"
0xbd="CP REGISTERL"              , 8"
0xbe="CP (REGISTER+$)"          , 19"   rd="16"
0xcb="shift CB"                    , 0"
0xdd="ignore"                      , 4"
0xe1="POP REGISTER"                , 14"   rd="8,11"
0xe3="EX (SP),REGISTER"            , 23"   rd="8,11"   wr="15,18"
0xe5="PUSH REGISTER"               , 15"   wr="9,12"
0xe9="JP REGISTER"                 , 8"
0xed="reset ED"                    , 4"
0xf9="LD SP,REGISTER"              , 10"
0xfd="ignore"                      , 4"

opcodes_cb

0x00="RLC B"                       , 8"
0x01="RLC C"                       , 8"
0x02="RLC D"                       , 8"
0x03="RLC E"                       , 8"
0x04="RLC H"                       , 8"
0x05="RLC L"                       , 8"
0x06="RLC (HL)"                    , 15"   rd="8"   wr="12"
0x07="RLC A"                       , 8"
0x08="RRC B"                       , 8"
0x09="RRC C"                       , 8"
0x0a="RRC D"                       , 8"
0x0b="RRC E"                       , 8"
0x0c="RRC H"                       , 8"
0x0d="RRC L"                       , 8"
0x0e="RRC (HL)"                    , 15"   rd="8"   wr="12"
0x0f="RRC A"                       , 8"
0x10="RL B"                        , 8"
0x11="RL C"                        , 8"
0x12="RL D"                        , 8"
0x13="RL E"                        , 8"
0x14="RL H"                        , 8"
0x15="RL L"                        , 8"
0x16="RL (HL)"                     , 15"   rd="8"   wr="12"
0x17="RL A"                        , 8"
0x18="RR B"                        , 8"
0x19="RR C"                        , 8"
0x1a="RR D"                        , 8"
0x1b="RR E"                        , 8"
0x1c="RR H"                        , 8"
0x1d="RR L"                        , 8"
0x1e="RR (HL)"                     , 15"   rd="8"   wr="12"
0x1f="RR A"                        , 8"
0x20="SLA B"                       , 8"
0x21="SLA C"                       , 8"
0x22="SLA D"                       , 8"
0x23="SLA E"                       , 8"
0x24="SLA H"                       , 8"
0x25="SLA L"                       , 8"
0x26="SLA (HL)"                    , 15"   rd="8"   wr="12"
0x27="SLA A"                       , 8"
0x28="SRA B"                       , 8"
0x29="SRA C"                       , 8"
0x2a="SRA D"                       , 8"
0x2b="SRA E"                       , 8"
0x2c="SRA H"                       , 8"
0x2d="SRA L"                       , 8"
0x2e="SRA (HL)"                    , 15"   rd="8"   wr="12"
0x2f="SRA A"                       , 8"
0x30="SLL B"                       , 8"
0x31="SLL C"                       , 8"
0x32="SLL D"                       , 8"
0x33="SLL E"                       , 8"
0x34="SLL H"                       , 8"
0x35="SLL L"                       , 8"
0x36="SLL (HL)"                    , 15"   rd="8"   wr="12"
0x37="SLL A"                       , 8"
0x38="SRL B"                       , 8"
0x39="SRL C"                       , 8"
0x3a="SRL D"                       , 8"
0x3b="SRL E"                       , 8"
0x3c="SRL H"                       , 8"
0x3d="SRL L"                       , 8"
0x3e="SRL (HL)"                    , 15"   rd="8"   wr="12"
0x3f="SRL A"                       , 8"
0x40="BIT 0,B"                     , 8"
0x41="BIT 0,C"                     , 8"
0x42="BIT 0,D"                     , 8"
0x43="BIT 0,E"                     , 8"
0x44="BIT 0,H"                     , 8"
0x45="BIT 0,L"                     , 8"
0x46="BIT 0,(HL)"                  , 12"   rd="8"
0x47="BIT 0,A"                     , 8"
0x48="BIT 1,B"                     , 8"
0x49="BIT 1,C"                     , 8"
0x4a="BIT 1,D"                     , 8"
0x4b="BIT 1,E"                     , 8"
0x4c="BIT 1,H"                     , 8"
0x4d="BIT 1,L"                     , 8"
0x4e="BIT 1,(HL)"                  , 12"   rd="8"
0x4f="BIT 1,A"                     , 8"
0x50="BIT 2,B"                     , 8"
0x51="BIT 2,C"                     , 8"
0x52="BIT 2,D"                     , 8"
0x53="BIT 2,E"                     , 8"
0x54="BIT 2,H"                     , 8"
0x55="BIT 2,L"                     , 8"
0x56="BIT 2,(HL)"                  , 12"   rd="8"
0x57="BIT 2,A"                     , 8"
0x58="BIT 3,B"                     , 8"
0x59="BIT 3,C"                     , 8"
0x5a="BIT 3,D"                     , 8"
0x5b="BIT 3,E"                     , 8"
0x5c="BIT 3,H"                     , 8"
0x5d="BIT 3,L"                     , 8"
0x5e="BIT 3,(HL)"                  , 12"   rd="8"
0x5f="BIT 3,A"                     , 8"
0x60="BIT 4,B"                     , 8"
0x61="BIT 4,C"                     , 8"
0x62="BIT 4,D"                     , 8"
0x63="BIT 4,E"                     , 8"
0x64="BIT 4,H"                     , 8"
0x65="BIT 4,L"                     , 8"
0x66="BIT 4,(HL)"                  , 12"   rd="8"
0x67="BIT 4,A"                     , 8"
0x68="BIT 5,B"                     , 8"
0x69="BIT 5,C"                     , 8"
0x6a="BIT 5,D"                     , 8"
0x6b="BIT 5,E"                     , 8"
0x6c="BIT 5,H"                     , 8"
0x6d="BIT 5,L"                     , 8"
0x6e="BIT 5,(HL)"                  , 12"   rd="8"
0x6f="BIT 5,A"                     , 8"
0x70="BIT 6,B"                     , 8"
0x71="BIT 6,C"                     , 8"
0x72="BIT 6,D"                     , 8"
0x73="BIT 6,E"                     , 8"
0x74="BIT 6,H"                     , 8"
0x75="BIT 6,L"                     , 8"
0x76="BIT 6,(HL)"                  , 12"   rd="8"
0x77="BIT 6,A"                     , 8"
0x78="BIT 7,B"                     , 8"
0x79="BIT 7,C"                     , 8"
0x7a="BIT 7,D"                     , 8"
0x7b="BIT 7,E"                     , 8"
0x7c="BIT 7,H"                     , 8"
0x7d="BIT 7,L"                     , 8"
0x7e="BIT 7,(HL)"                  , 12"   rd="8"
0x7f="BIT 7,A"                     , 8"
0x80="RES 0,B"                     , 8"
0x81="RES 0,C"                     , 8"
0x82="RES 0,D"                     , 8"
0x83="RES 0,E"                     , 8"
0x84="RES 0,H"                     , 8"
0x85="RES 0,L"                     , 8"
0x86="RES 0,(HL)"                  , 15"   rd="8"   wr="12"
0x87="RES 0,A"                     , 8"
0x88="RES 1,B"                     , 8"
0x89="RES 1,C"                     , 8"
0x8a="RES 1,D"                     , 8"
0x8b="RES 1,E"                     , 8"
0x8c="RES 1,H"                     , 8"
0x8d="RES 1,L"                     , 8"
0x8e="RES 1,(HL)"                  , 15"   rd="8"   wr="12"
0x8f="RES 1,A"                     , 8"
0x90="RES 2,B"                     , 8"
0x91="RES 2,C"                     , 8"
0x92="RES 2,D"                     , 8"
0x93="RES 2,E"                     , 8"
0x94="RES 2,H"                     , 8"
0x95="RES 2,L"                     , 8"
0x96="RES 2,(HL)"                  , 15"   rd="8"   wr="12"
0x97="RES 2,A"                     , 8"
0x98="RES 3,B"                     , 8"
0x99="RES 3,C"                     , 8"
0x9a="RES 3,D"                     , 8"
0x9b="RES 3,E"                     , 8"
0x9c="RES 3,H"                     , 8"
0x9d="RES 3,L"                     , 8"
0x9e="RES 3,(HL)"                  , 15"   rd="8"   wr="12"
0x9f="RES 3,A"                     , 8"
0xa0="RES 4,B"                     , 8"
0xa1="RES 4,C"                     , 8"
0xa2="RES 4,D"                     , 8"
0xa3="RES 4,E"                     , 8"
0xa4="RES 4,H"                     , 8"
0xa5="RES 4,L"                     , 8"
0xa6="RES 4,(HL)"                  , 15"   rd="8"   wr="12"
0xa7="RES 4,A"                     , 8"
0xa8="RES 5,B"                     , 8"
0xa9="RES 5,C"                     , 8"
0xaa="RES 5,D"                     , 8"
0xab="RES 5,E"                     , 8"
0xac="RES 5,H"                     , 8"
0xad="RES 5,L"                     , 8"
0xae="RES 5,(HL)"                  , 15"   rd="8"   wr="12"
0xaf="RES 5,A"                     , 8"
0xb0="RES 6,B"                     , 8"
0xb1="RES 6,C"                     , 8"
0xb2="RES 6,D"                     , 8"
0xb3="RES 6,E"                     , 8"
0xb4="RES 6,H"                     , 8"
0xb5="RES 6,L"                     , 8"
0xb6="RES 6,(HL)"                  , 15"   rd="8"   wr="12"
0xb7="RES 6,A"                     , 8"
0xb8="RES 7,B"                     , 8"
0xb9="RES 7,C"                     , 8"
0xba="RES 7,D"                     , 8"
0xbb="RES 7,E"                     , 8"
0xbc="RES 7,H"                     , 8"
0xbd="RES 7,L"                     , 8"
0xbe="RES 7,(HL)"                  , 15"   rd="8"   wr="12"
0xbf="RES 7,A"                     , 8"
0xc0="SET 0,B"                     , 8"
0xc1="SET 0,C"                     , 8"
0xc2="SET 0,D"                     , 8"
0xc3="SET 0,E"                     , 8"
0xc4="SET 0,H"                     , 8"
0xc5="SET 0,L"                     , 8"
0xc6="SET 0,(HL)"                  , 15"   rd="8"   wr="12"
0xc7="SET 0,A"                     , 8"
0xc8="SET 1,B"                     , 8"
0xc9="SET 1,C"                     , 8"
0xca="SET 1,D"                     , 8"
0xcb="SET 1,E"                     , 8"
0xcc="SET 1,H"                     , 8"
0xcd="SET 1,L"                     , 8"
0xce="SET 1,(HL)"                  , 15"   rd="8"   wr="12"
0xcf="SET 1,A"                     , 8"
0xd0="SET 2,B"                     , 8"
0xd1="SET 2,C"                     , 8"
0xd2="SET 2,D"                     , 8"
0xd3="SET 2,E"                     , 8"
0xd4="SET 2,H"                     , 8"
0xd5="SET 2,L"                     , 8"
0xd6="SET 2,(HL)"                  , 15"   rd="8"   wr="12"
0xd7="SET 2,A"                     , 8"
0xd8="SET 3,B"                     , 8"
0xd9="SET 3,C"                     , 8"
0xda="SET 3,D"                     , 8"
0xdb="SET 3,E"                     , 8"
0xdc="SET 3,H"                     , 8"
0xdd="SET 3,L"                     , 8"
0xde="SET 3,(HL)"                  , 15"   rd="8"   wr="12"
0xdf="SET 3,A"                     , 8"
0xe0="SET 4,B"                     , 8"
0xe1="SET 4,C"                     , 8"
0xe2="SET 4,D"                     , 8"
0xe3="SET 4,E"                     , 8"
0xe4="SET 4,H"                     , 8"
0xe5="SET 4,L"                     , 8"
0xe6="SET 4,(HL)"                  , 15"   rd="8"   wr="12"
0xe7="SET 4,A"                     , 8"
0xe8="SET 5,B"                     , 8"
0xe9="SET 5,C"                     , 8"
0xea="SET 5,D"                     , 8"
0xeb="SET 5,E"                     , 8"
0xec="SET 5,H"                     , 8"
0xed="SET 5,L"                     , 8"
0xee="SET 5,(HL)"                  , 15"   rd="8"   wr="12"
0xef="SET 5,A"                     , 8"
0xf0="SET 6,B"                     , 8"
0xf1="SET 6,C"                     , 8"
0xf2="SET 6,D"                     , 8"
0xf3="SET 6,E"                     , 8"
0xf4="SET 6,H"                     , 8"
0xf5="SET 6,L"                     , 8"
0xf6="SET 6,(HL)"                  , 15"   rd="8"   wr="12"
0xf7="SET 6,A"                     , 8"
0xf8="SET 7,B"                     , 8"
0xf9="SET 7,C"                     , 8"
0xfa="SET 7,D"                     , 8"
0xfb="SET 7,E"                     , 8"
0xfc="SET 7,H"                     , 8"
0xfd="SET 7,L"                     , 8"
0xfe="SET 7,(HL)"                  , 15"   rd="8"   wr="12"
0xff="SET 7,A"                     , 8"


# ddcb & fdcb, (for ddcb REGISTER=IX, for fdcb REGISTER=IY) 256
# empty lines with opcode around 0x40 means "same as the next non-blank opcode"

0x00="LD B,RLC (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x01="LD C,RLC (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x02="LD D,RLC (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x03="LD E,RLC (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x04="LD H,RLC (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x05="LD L,RLC (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x06="RLC (REGISTER+$)"           , 23"   rd="16"   wr="20"
0x07="LD A,RLC (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x08="LD B,RRC (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x09="LD C,RRC (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x0a="LD D,RRC (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x0b="LD E,RRC (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x0c="LD H,RRC (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x0d="LD L,RRC (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x0e="RRC (REGISTER+$)"           , 23"   rd="16"   wr="20"
0x0f="LD A,RRC (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x10="LD B,RL (REGISTER+$)"       , 23"   rd="16"   wr="20"
0x11="LD C,RL (REGISTER+$)"       , 23"   rd="16"   wr="20"
0x12="LD D,RL (REGISTER+$)"       , 23"   rd="16"   wr="20"
0x13="LD E,RL (REGISTER+$)"       , 23"   rd="16"   wr="20"
0x14="LD H,RL (REGISTER+$)"       , 23"   rd="16"   wr="20"
0x15="LD L,RL (REGISTER+$)"       , 23"   rd="16"   wr="20"
0x16="RL (REGISTER+$)"            , 23"   rd="16"   wr="20"
0x17="LD A,RL (REGISTER+$)"       , 23"   rd="16"   wr="20"
0x18="LD B,RR (REGISTER+$)"       , 23"   rd="16"   wr="20"
0x19="LD C,RR (REGISTER+$)"       , 23"   rd="16"   wr="20"
0x1a="LD D,RR (REGISTER+$)"       , 23"   rd="16"   wr="20"
0x1b="LD E,RR (REGISTER+$)"       , 23"   rd="16"   wr="20"
0x1c="LD H,RR (REGISTER+$)"       , 23"   rd="16"   wr="20"
0x1d="LD L,RR (REGISTER+$)"       , 23"   rd="16"   wr="20"
0x1e="RR (REGISTER+$)"            , 23"   rd="16"   wr="20"
0x1f="LD A,RR (REGISTER+$)"       , 23"   rd="16"   wr="20"
0x20="LD B,SLA (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x21="LD C,SLA (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x22="LD D,SLA (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x23="LD E,SLA (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x24="LD H,SLA (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x25="LD L,SLA (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x26="SLA (REGISTER+$)"           , 23"   rd="16"   wr="20"
0x27="LD A,SLA (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x28="LD B,SRA (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x29="LD C,SRA (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x2a="LD D,SRA (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x2b="LD E,SRA (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x2c="LD H,SRA (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x2d="LD L,SRA (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x2e="SRA (REGISTER+$)"           , 23"   rd="16"   wr="20"
0x2f="LD A,SRA (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x30="LD B,SLL (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x31="LD C,SLL (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x32="LD D,SLL (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x33="LD E,SLL (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x34="LD H,SLL (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x35="LD L,SLL (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x36="SLL (REGISTER+$)"           , 23"   rd="16"   wr="20"
0x37="LD A,SLL (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x38="LD B,SRL (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x39="LD C,SRL (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x3a="LD D,SRL (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x3b="LD E,SRL (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x3c="LD H,SRL (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x3d="LD L,SRL (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x3e="SRL (REGISTER+$)"           , 23"   rd="16"   wr="20"
0x3f="LD A,SRL (REGISTER+$)"      , 23"   rd="16"   wr="20"
0x40
0x41
0x42
0x43
0x44
0x45
0x46
0x47="BIT 0,(REGISTER+$)"         , 20"   rd="16"
0x48
0x49
0x4a
0x4b
0x4c
0x4d
0x4e
0x4f="BIT 1,(REGISTER+$)"         , 20"   rd="16"
0x50
0x51
0x52
0x53
0x54
0x55
0x56
0x57="BIT 2,(REGISTER+$)"         , 20"   rd="16"
0x58
0x59
0x5a
0x5b
0x5c
0x5d
0x5e
0x5f="BIT 3,(REGISTER+$)"         , 20"   rd="16"
0x60
0x61
0x62
0x63
0x64
0x65
0x66
0x67="BIT 4,(REGISTER+$)"         , 20"   rd="16"
0x68
0x69
0x6a
0x6b
0x6c
0x6d
0x6e
0x6f="BIT 5,(REGISTER+$)"         , 20"   rd="16"
0x70
0x71
0x72
0x73
0x74
0x75
0x76
0x77="BIT 6,(REGISTER+$)"         , 20"   rd="16"
0x78
0x79
0x7a
0x7b
0x7c
0x7d
0x7e
0x7f="BIT 7,(REGISTER+$)"         , 20"   rd="16"
0x80="LD B,RES 0,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x81="LD C,RES 0,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x82="LD D,RES 0,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x83="LD E,RES 0,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x84="LD H,RES 0,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x85="LD L,RES 0,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x86="RES 0,(REGISTER+$)"         , 23"   rd="16"   wr="20"
0x87="LD A,RES 0,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x88="LD B,RES 1,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x89="LD C,RES 1,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x8a="LD D,RES 1,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x8b="LD E,RES 1,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x8c="LD H,RES 1,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x8d="LD L,RES 1,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x8e="RES 1,(REGISTER+$)"         , 23"   rd="16"   wr="20"
0x8f="LD A,RES 1,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x90="LD B,RES 2,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x91="LD C,RES 2,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x92="LD D,RES 2,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x93="LD E,RES 2,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x94="LD H,RES 2,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x95="LD L,RES 2,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x96="RES 2,(REGISTER+$)"         , 23"   rd="16"   wr="20"
0x97="LD A,RES 2,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x98="LD B,RES 3,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x99="LD C,RES 3,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x9a="LD D,RES 3,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x9b="LD E,RES 3,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x9c="LD H,RES 3,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x9d="LD L,RES 3,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0x9e="RES 3,(REGISTER+$)"         , 23"   rd="16"   wr="20"
0x9f="LD A,RES 3,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xa0="LD B,RES 4,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xa1="LD C,RES 4,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xa2="LD D,RES 4,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xa3="LD E,RES 4,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xa4="LD H,RES 4,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xa5="LD L,RES 4,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xa6="RES 4,(REGISTER+$)"         , 23"   rd="16"   wr="20"
0xa7="LD A,RES 4,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xa8="LD B,RES 5,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xa9="LD C,RES 5,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xaa="LD D,RES 5,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xab="LD E,RES 5,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xac="LD H,RES 5,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xad="LD L,RES 5,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xae="RES 5,(REGISTER+$)"         , 23"   rd="16"   wr="20"
0xaf="LD A,RES 5,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xb0="LD B,RES 6,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xb1="LD C,RES 6,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xb2="LD D,RES 6,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xb3="LD E,RES 6,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xb4="LD H,RES 6,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xb5="LD L,RES 6,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xb6="RES 6,(REGISTER+$)"         , 23"   rd="16"   wr="20"
0xb7="LD A,RES 6,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xb8="LD B,RES 7,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xb9="LD C,RES 7,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xba="LD D,RES 7,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xbb="LD E,RES 7,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xbc="LD H,RES 7,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xbd="LD L,RES 7,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xbe="RES 7,(REGISTER+$)"         , 23"   rd="16"   wr="20"
0xbf="LD A,RES 7,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xc0="LD B,SET 0,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xc1="LD C,SET 0,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xc2="LD D,SET 0,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xc3="LD E,SET 0,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xc4="LD H,SET 0,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xc5="LD L,SET 0,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xc6="SET 0,(REGISTER+$)"         , 23"   rd="16"   wr="20"
0xc7="LD A,SET 0,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xc8="LD B,SET 1,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xc9="LD C,SET 1,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xca="LD D,SET 1,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xcb="LD E,SET 1,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xcc="LD H,SET 1,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xcd="LD L,SET 1,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xce="SET 1,(REGISTER+$)"         , 23"   rd="16"   wr="20"
0xcf="LD A,SET 1,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xd0="LD B,SET 2,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xd1="LD C,SET 2,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xd2="LD D,SET 2,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xd3="LD E,SET 2,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xd4="LD H,SET 2,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xd5="LD L,SET 2,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xd6="SET 2,(REGISTER+$)"         , 23"   rd="16"   wr="20"
0xd7="LD A,SET 2,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xd8="LD B,SET 3,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xd9="LD C,SET 3,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xda="LD D,SET 3,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xdb="LD E,SET 3,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xdc="LD H,SET 3,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xdd="LD L,SET 3,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xde="SET 3,(REGISTER+$)"         , 23"   rd="16"   wr="20"
0xdf="LD A,SET 3,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xe0="LD B,SET 4,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xe1="LD C,SET 4,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xe2="LD D,SET 4,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xe3="LD E,SET 4,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xe4="LD H,SET 4,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xe5="LD L,SET 4,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xe6="SET 4,(REGISTER+$)"         , 23"   rd="16"   wr="20"
0xe7="LD A,SET 4,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xe8="LD B,SET 5,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xe9="LD C,SET 5,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xea="LD D,SET 5,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xeb="LD E,SET 5,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xec="LD H,SET 5,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xed="LD L,SET 5,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xee="SET 5,(REGISTER+$)"         , 23"   rd="16"   wr="20"
0xef="LD A,SET 5,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xf0="LD B,SET 6,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xf1="LD C,SET 6,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xf2="LD D,SET 6,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xf3="LD E,SET 6,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xf4="LD H,SET 6,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xf5="LD L,SET 6,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xf6="SET 6,(REGISTER+$)"         , 23"   rd="16"   wr="20"
0xf7="LD A,SET 6,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xf8="LD B,SET 7,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xf9="LD C,SET 7,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xfa="LD D,SET 7,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xfb="LD E,SET 7,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xfc="LD H,SET 7,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xfd="LD L,SET 7,(REGISTER+$)"    , 23"   rd="16"   wr="20"
0xfe="SET 7,(REGISTER+$)"         , 23"   rd="16"   wr="20"
0xff="LD A,SET 7,(REGISTER+$)"    , 23"   rd="16"   wr="20"


#ED opcodes
#all codes not listed here is NOP (NOP that eats 8 t-states instead of 4)

0x40="IN B,(C)"                    , 12"   rd="9"
0x41="OUT (C),B"                   , 12"   wr="9"
0x42="SBC HL,BC"                   , 15"
0x43="LD (@),BC"                , 20"   wr="14,17"
0x44="NEG"                         , 8"
0x45="RETN"                        , 14"   rd="8,11"
0x46="IM 0"                        , 8"
0x47="LD I,A"                      , 9"
0x48="IN C,(C)"                    , 12"   rd="9"
0x49="OUT (C),C"                   , 12"   wr="9"
0x4a="ADC HL,BC"                   , 15"
0x4b="LD BC,(@)"                , 20"   rd="14,17"
0x4c="NEG"                         , 8"
0x4d="RETI"                        , 14"   rd="8,11"
0x4e="IM 0"                        , 8"
0x4f="LD R,A"                      , 9"
0x50="IN D,(C)"                    , 12"   rd="9"
0x51="OUT (C),D"                   , 12"   wr="9"
0x52="SBC HL,DE"                   , 15"
0x53="LD (@),DE"                , 20"   wr="14,17"
0x54="NEG"                         , 8"
0x55="RETN"                        , 14"   rd="8,11"
0x56="IM 1"                        , 8"
0x57="LD A,I"                      , 9"
0x58="IN E,(C)"                    , 12"   rd="9"
0x59="OUT (C),E"                   , 12"   wr="9"
0x5a="ADC HL,DE"                   , 15"
0x5b="LD DE,(@)"                , 20"   rd="14,17"
0x5c="NEG"                         , 8"
0x5d="RETI"                        , 14"   rd="8,11"
0x5e="IM 2"                        , 8"
0x5f="LD A,R"                      , 9"
0x60="IN H,(C)"                    , 12"   rd="9"
0x61="OUT (C),H"                   , 12"   wr="9"
0x62="SBC HL,HL"                   , 15"
0x63="LD (@),HL"                , 20"   wr="14,17"
0x64="NEG"                         , 8"
0x65="RETN"                        , 14"   rd="8,11"
0x66="IM 0"                        , 8"
0x67="RRD"                         , 18"   rd="8"   wr="15"
0x68="IN L,(C)"                    , 12"   rd="9"
0x69="OUT (C),L"                   , 12"   wr="9"
0x6a="ADC HL,HL"                   , 15"
0x6b="LD HL,(@)"                , 20"   rd="14,17"
0x6c="NEG"                         , 8"
0x6d="RETI"                        , 14"   rd="8,11"
0x6e="IM 0"                        , 8"
0x6f="RLD"                         , 18"   rd="8"   wr="15"
0x70="IN_F (C)"                    , 12"   rd="9"
0x71="OUT (C),0"                   , 12"   wr="9"
0x72="SBC HL,SP"                   , 15"
0x73="LD (@),SP"                , 20"   wr="14,17"
0x74="NEG"                         , 8"
0x75="RETN"                        , 14"   rd="8,11"
0x76="IM 1"                        , 8"
0x78="IN A,(C)"                    , 12"   rd="9"
0x79="OUT (C),A"                   , 12"   wr="9"
0x7a="ADC HL,SP"                   , 15"
0x7b="LD SP,(@)"                , 20"   rd="14,17"
0x7c="NEG"                         , 8"
0x7d="RETI"                        , 14"   rd="8,11"
0x7e="IM 2"                        , 8"
0xa0="LDI"                         , 16"   rd="8"   wr="11"
0xa1="CPI"                         , 16"   rd="8"
0xa2="INI"                         , 16"   rd="10"   wr="13"
0xa3="OUTI"                        , 16"   rd="9"   wr="13"
0xa8="LDD"                         , 16"   rd="8"   wr="11"
0xa9="CPD"                         , 16"   rd="8"
0xaa="IND"                         , 16"   rd="10"   wr="13"
0xab="OUTD"                        , 16"   rd="9"   wr="13"
0xb0="LDIR"                        , 16/21"   rd="8"    wr="11"
0xb1="CPIR"                        , 16/21"   rd="8"
0xb2="INIR"                        , 16/21"   rd="10"    wr="13"
0xb3="OTIR"                        , 16/21"   rd="9"    wr="13"
0xb8="LDDR"                        , 16/21"   rd="8"    wr="11"
0xb9="CPDR"                        , 16/21"   rd="8"
0xba="INDR"                        , 16/21"   rd="10"    wr="13"
0xbb="OTDR"                        , 16/21"   rd="9"    wr="13"

*/
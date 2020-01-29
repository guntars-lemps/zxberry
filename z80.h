#ifndef Z80_H
#define Z80_H

#include <stdint.h>
#include <stdbool.h>
#include "memory.h"
#include "ports.h"

typedef struct
{
    uint16_t af, bc, de, hl, af_, bc_, de_, hl_, ix, iy;
} t_r16;

# if defined __BIG_ENDIAN__

typedef struct
{
    uint8_t a, f, b, c, d, e, h, l, a_, f_, b_, c_, d_, e_, h_, l_, ixh, ixl, iyh, iyl;
} t_r8;

#else

typedef struct
{
    uint8_t f, a, c, b, e, d, l, h, f_, a_, c_, b_, e_, d_, l_, h_, ixl, ixh, iyl, iyh;
} t_r8;

#endif

enum t_interrupt_mode {IM0, IM1, IM2};

typedef struct
{
    uint16_t pc, sp, memptr;
    uint8_t i, r, r7, q;
    uint8_t iff1, iff2;
    enum t_interrupt_mode im;
    bool halted;
    union
    {
        t_r16 r16;
        t_r8 r8;
    };
    int states;
    int shifts;
    bool irq;

    t_memory_reader mr;
    t_memory_writer mw;
    t_port_reader pr;
    t_port_writer pw;

} t_z80;

t_z80 z80;

void z80_init();

#endif
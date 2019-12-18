#ifndef Z80_H
#define Z80_H

#include <stdint.h>
#include <stdbool.h>


typedef struct
{
    uint16_t af, bc, de, hl, af_, bc_, de_, hl_, ix, iy;
} t_r16;

# if defined __BIG_ENDIAN__

typedef struct
{
    uint8_t a, f, b, c, d, e, h, l, a_, f_, b_, c_, d_, e_, h_, l_, ixh, x, iyh, y;
} t_r8;

#else

typedef struct
{
    uint8_t f, a, c, b, e, d, l, h, f_, a_, c_, b_, e_, d_, l_, h_, x, ixh, y, iyh;
} t_r8;

#endif

enum t_interrupt_mode {IM0, IM1, IM2};

typedef struct
{
    uint16_t pc, sp, memptr;
    uint8_t i, r, q;
    uint8_t iff1, iff2;
    enum t_interrupt_mode im;
    bool halted;
    union
    {
        t_r16 r16;
        t_r8 r8;
    };
} t_z80;

t_z80 z80;

void z80_init();

#endif
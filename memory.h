#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

typedef uint8_t (*t_memory_reader)(uint16_t address);
typedef void (*t_memory_writer)(uint16_t address, uint8_t byte);


#endif
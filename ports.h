#ifndef PORTS_H
#define PORTS_H


#include <stdint.h>

typedef uint8_t (*t_port_reader)(uint16_t address);
typedef void (*t_port_writer)(uint16_t address, uint8_t byte);

#endif
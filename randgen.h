#ifndef _RANDGEN_H
#define _RANDGEN_H
#include <stdint.h>

void init_randgen();
uint8_t rand8();
uint16_t rand16();
uint32_t rand32();
#endif
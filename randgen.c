#include "randgen.h"
#include <time.h>
#include <stdlib.h>

void init_randgen() {
    srand(time(0));
}

uint8_t rand8() {
    return rand() % 256;
}

uint16_t rand16() {
    return rand() % 65536;
}

uint32_t rand32() {
    return rand() % 4294967296;
}
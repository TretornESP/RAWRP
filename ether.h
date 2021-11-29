#ifndef _ETHER_H
#define _ETHER_H
#include <stdint.h>

struct eth {/*
    uint8_t preamble[7];
    uint8_t sfd;*/
    uint8_t da[6];
    uint8_t sa[6];
    uint8_t length[2];
    uint8_t *data;
    uint32_t crc;
} __attribute__((packed));

void eth(struct eth *eth, uint8_t* sa, uint8_t *da, uint8_t *data, uint8_t *length);

#endif
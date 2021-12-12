#ifndef _ETHER_H
#define _ETHER_H
#include <stdint.h>
#include <stdlib.h>

#define ETH_INVALID 0x1
#define ETH_II      0x2
#define ETH_802_3   0x3

#define ETH_MAX_FRAME_SIZE 1518
//We dont support jumbo frames right now

//#define INCLUDE_PREAMBLE //Only if your nic is weird
#ifdef INCLUDE_PREAMBLE
#define ETH_PAYLOAD_SIZE_OFFSET 20
#else
#define ETH_PAYLOAD_SIZE_OFFSET 12
#endif


struct eth {/*
    uint8_t preamble[7];
    uint8_t sfd;*/
    uint8_t da[6];
    uint8_t sa[6];
    uint8_t length[2];
    uint8_t *data;
    uint32_t crc;
} __attribute__((packed));

void parse_eth(struct eth *eth, uint8_t* data);
uint8_t get_eth_version(struct eth *eth);
void get_eth_data(struct eth *eth, uint8_t *data, uint16_t size);
void init_eth(struct eth *eth, uint8_t* sa, uint8_t *da, uint8_t *data, uint8_t *type, uint8_t *length);
void destroy_eth(struct eth *eth);
size_t size_eth(struct eth *eth);

#endif


#ifndef _ICMP_H
#define _ICMP_H

#include <stdint.h>

#define ECHO_REQUEST 0x08
#define ECHO_REPLY 0x00

struct icmp{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint64_t *data;
};

uint16_t calculate_icmp_checksum(uint8_t type, uint8_t code, uint16_t *data, size_t len);
void init_icmp(struct icmp *icmp, uint8_t type, uint8_t code, uint8_t* data, size_t len);

#endif
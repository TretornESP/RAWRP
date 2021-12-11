#ifndef _ICMP_H
#define _ICMP_H

#include <string.h>
#include <stdint.h>

#define ICMP_ECHO_REQUEST 0x08
#define ICMP_ECHO_REPLY 0x00
#define ICMP_ID     0x69
#define ICMP_REQUEST_DATA_SIZE 32

struct icmp{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t id;
    uint16_t seq;
    uint8_t *data;
}  __attribute__((packed));

uint16_t calculate_icmp_checksum(uint8_t type, uint8_t code, uint16_t *data, size_t len);
void init_icmp(struct icmp *icmp, uint8_t type, uint8_t code);
void data_icmp(struct icmp *icmp, uint8_t* data);
void destroy_icmp(struct icmp *icmp);
size_t icmp_size(struct icmp *icmp);
#endif
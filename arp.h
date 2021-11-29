#ifndef _ARP_H
#define _ARP_H
#include <stdint.h>
struct arp {
    uint16_t htype;
    uint16_t ptype;
    uint8_t hlen;
    uint8_t plen;
    uint16_t oper;
    uint8_t *sha;
    uint8_t *spa;
    uint8_t *tha;
    uint8_t *tpa;
} __attribute__((packed));

void init_arp(struct arp* arp, uint8_t* mac, uint8_t* ip);
void destroy_arp(struct arp* arp);

#endif
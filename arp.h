#ifndef _ARP_H
#define _ARP_H

#define ARP_REQUEST 1
#define ARP_REPLY 2

#include <stdint.h>
#include <unistd.h>

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

struct arp_cache {
    uint8_t hlen;
    uint8_t plen;
    uint8_t *ip;
    uint8_t *mac;
    struct arp_cache *next;
};

void init_arp(struct arp* arp, uint8_t* mac, uint8_t* sip, const char* tips);
void parse_arp(struct arp* arp, uint8_t* packet, ssize_t size);
void destroy_arp(struct arp* arp);
void size_arp(struct arp* arp, uint8_t* size);
void ethertype_arp(uint8_t *type);
void data_arp(struct arp* arp, uint8_t* data);
uint8_t get_arp_payload_size(void * buffer);

void init_arp_cache(struct arp_cache* cache);
void destroy_arp_cache(struct arp_cache* cache);
void add_arp_cache(struct arp_cache** cache, uint8_t* ip, uint8_t* mac, uint8_t hlen, uint8_t plen);
//int overwrite_arp_cache(struct arp_cache** cache, uint8_t* ip, uint8_t* mac, uint8_t hlen, uint8_t plen);
void remove_arp_cache(struct arp_cache** cache, uint8_t* ip, uint8_t plen);
int get_arp_cache(struct arp_cache** cache, uint8_t* ip, uint8_t* mac, uint8_t hlen, uint8_t plen);
struct arp_cache* get_global_arp_cache();
void print_arp_cache(struct arp_cache* cache);
#endif